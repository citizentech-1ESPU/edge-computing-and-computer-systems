// Xepa Certa - GS 2026.2 CitizenTech - Edge Computing
// Time SafeTech
// Pedro Thyago de Araujo Santos - RM 570939
// Pedro Passos Corsini - RM 573493
// Daniel Gomes Torres - RM 573436
// Henrique Lira - RM 571009
//
// Sensor na caixa termica do feirante. Soma o tempo que o produto
// ficou acima de 5 C (regra das 2h/4h) e decide sozinho o que fazer,
// mesmo sem internet. Com wifi, manda os dados pro ThingSpeak.

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#define PINO_SENSOR 4
#define LED_VERDE 27
#define LED_AMARELO 26
#define LED_VERMELHO 25
#define BUZZER 2
#define BOTAO 13

const float LIMITE_FRIO_C = 5.0;
const float XEPA_APOS_MIN = 120;      // 2h fora do frio
const float DESCARTE_APOS_MIN = 240;  // 4h fora do frio
const float AVISO_GELO_MIN = 15;      // avisa 15 min antes de passar de 5 C
const float TEMP_MIN_VALIDA = -30;
const float TEMP_MAX_VALIDA = 60;
const int LEITURAS_INVALIDAS_PARA_ERRO = 3;
const int JANELA_MEDIA = 5;

// 1 segundo da simulacao vale 2 minutos, pra caber as 4h no video.
// Na caixa de verdade isso seria 1.
constexpr float FATOR_TEMPO = 120;

// a taxa compara com a leitura de uns 5 minutos atras
constexpr int JANELA_TAXA = 5 * 60 / FATOR_TEMPO + 0.5;

const char* THINGSPEAK_KEY = "COLOQUE_SUA_WRITE_API_KEY";

enum Estado { NORMAL, GELO, XEPA, DESCARTE, SEM_LEITURA };

OneWire oneWire(PINO_SENSOR);
DallasTemperature sensor(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

Estado estado = NORMAL;
Estado estadoAnterior = SEM_LEITURA;

float leituras[JANELA_MEDIA];
int qtdLeituras = 0, posLeitura = 0;
float hist[JANELA_TAXA];
int qtdHist = 0, posHist = 0;

float temp = 0;
float media = 0;
float taxa = 0;         // C por hora
float minutosFora = 0;
int errosSeguidos = 0;

unsigned long tLeitura = 0, tPisca = 0, tEnvio = 0, tAlarme = 0, tBotao = 0;
bool pisca = false;

// fila de bipes, pra tocar 2 ou 3 seguidos sem usar delay
int bipesFaltando = 0, freqBip = 0, duracaoBip = 0;
unsigned long tProxBip = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BOTAO, INPUT_PULLUP);

  Wire.begin(22, 21);  // SDA no 22, SCL no 21
  lcd.init();
  lcd.backlight();
  lcd.print("Xepa Certa");

  sensor.begin();
  sensor.setWaitForConversion(false);  // pra nao travar o loop
  sensor.requestTemperatures();

  WiFi.begin("Wokwi-GUEST", "", 6);  // nao espera conectar, funciona offline

  Serial.println("Xepa Certa ligada");
}

void loop() {
  unsigned long agora = millis();

  verificarBotao(agora);

  if (agora - tLeitura >= 1000) {
    float seg = (agora - tLeitura) / 1000.0;
    tLeitura = agora;

    lerSensor(seg);
    estado = decidirEstado();

    if (estado != estadoAnterior) {
      mudouEstado(agora);
      estadoAnterior = estado;
    }

    atualizarDisplay();
    mostrarSerial();
  }

  atualizarLeds(agora);
  atualizarBuzzer(agora);

  if (agora - tEnvio >= 20000) {  // ThingSpeak so aceita a cada 15 s
    tEnvio = agora;
    enviarThingSpeak();
  }
}

void lerSensor(float seg) {
  float t = sensor.getTempCByIndex(0);
  sensor.requestTemperatures();

  // -127 = sensor solto, 85 = erro do DS18B20 ao ligar
  if (t == DEVICE_DISCONNECTED_C || t == 85.0 || t < TEMP_MIN_VALIDA || t > TEMP_MAX_VALIDA) {
    errosSeguidos++;
    Serial.print("Leitura invalida ignorada: ");
    Serial.println(t);
    return;
  }

  errosSeguidos = 0;
  temp = t;
  media = suavizar(t);
  taxa = calcularTaxa(media);

  if (media > LIMITE_FRIO_C) {
    minutosFora += seg * FATOR_TEMPO / 60.0;
  }
}

float suavizar(float valor) {
  leituras[posLeitura] = valor;
  posLeitura = (posLeitura + 1) % JANELA_MEDIA;
  if (qtdLeituras < JANELA_MEDIA) qtdLeituras++;

  float soma = 0;
  for (int i = 0; i < qtdLeituras; i++) soma += leituras[i];
  return soma / qtdLeituras;
}

float calcularTaxa(float m) {
  float resultado = 0;

  if (qtdHist == JANELA_TAXA) {
    float minutos = JANELA_TAXA * FATOR_TEMPO / 60.0;
    resultado = (m - hist[posHist]) / minutos * 60.0;
  }

  hist[posHist] = m;
  posHist = (posHist + 1) % JANELA_TAXA;
  if (qtdHist < JANELA_TAXA) qtdHist++;

  return resultado;
}

int minutosRestantes() {
  return max(0, (int)(DESCARTE_APOS_MIN - minutosFora));
}

Estado decidirEstado() {
  if (errosSeguidos >= LEITURAS_INVALIDAS_PARA_ERRO) return SEM_LEITURA;
  if (errosSeguidos > 0) return estado;  // 1 ou 2 erros: segura o estado

  if (minutosFora >= DESCARTE_APOS_MIN) return DESCARTE;
  if (minutosFora >= XEPA_APOS_MIN) return XEPA;
  if (media > LIMITE_FRIO_C) return GELO;

  if (taxa > 0) {
    float faltam = (LIMITE_FRIO_C - media) / taxa * 60.0;
    if (faltam <= AVISO_GELO_MIN) return GELO;
  }

  return NORMAL;
}

void atualizarLeds(unsigned long agora) {
  if (agora - tPisca >= 400) {
    tPisca = agora;
    pisca = !pisca;
  }

  bool vd = false, am = false, vm = false;

  switch (estado) {
    case NORMAL:      vd = true; break;
    case GELO:        vd = pisca; break;
    case XEPA:        am = true; break;
    case DESCARTE:    vm = true; break;
    case SEM_LEITURA: vd = am = vm = pisca; break;
  }

  digitalWrite(LED_VERDE, vd);
  digitalWrite(LED_AMARELO, am);
  digitalWrite(LED_VERMELHO, vm);
}

void bipar(int qtd, int freq, int duracao) {
  bipesFaltando = qtd;
  freqBip = freq;
  duracaoBip = duracao;
  tProxBip = millis();
}

// cada estado tem um som diferente
void tocarAlarme() {
  switch (estado) {
    case GELO:        bipar(1, 2000, 150); break;   // bipe curto
    case XEPA:        bipar(2, 1200, 150); break;   // dois bipes, so ao entrar
    case DESCARTE:    bipar(3, 600, 200); break;    // tres bipes
    case SEM_LEITURA: bipar(1, 300, 1000); break;   // bipe longo
    default: break;
  }
}

void atualizarBuzzer(unsigned long agora) {
  if (estado == GELO || estado == SEM_LEITURA) {
    if (agora - tAlarme >= 10000) {
      tAlarme = agora;
      tocarAlarme();
    }
  }
  if (estado == DESCARTE && agora - tAlarme >= 5000) {
    tAlarme = agora;
    tocarAlarme();
  }

  if (bipesFaltando > 0 && agora >= tProxBip) {
    tone(BUZZER, freqBip, duracaoBip);
    bipesFaltando--;
    tProxBip = agora + duracaoBip + 150;
  }
}

void mudouEstado(unsigned long agora) {
  tAlarme = agora;
  tocarAlarme();

  Serial.print(">> ");
  Serial.println(nomeEstado(estado));
}

void atualizarDisplay() {
  lcd.clear();

  if (estado == SEM_LEITURA) {
    lcd.print("SEM LEITURA");
    lcd.setCursor(0, 1);
    lcd.print("confira o sensor");
    return;
  }

  lcd.print(media, 1);
  lcd.print("C Fora ");
  lcd.print(formatarTempo(minutosFora));
  lcd.setCursor(0, 1);
  lcd.print(nomeEstado(estado));
}

void mostrarSerial() {
  if (estado == SEM_LEITURA) return;

  Serial.print("temp ");
  Serial.print(temp, 1);
  Serial.print(" | media ");
  Serial.print(media, 1);
  Serial.print(" | taxa ");
  Serial.print(taxa, 1);
  Serial.print(" C/h | fora ");
  Serial.print(formatarTempo(minutosFora));
  Serial.print(" | ");
  Serial.println(nomeEstado(estado));
}

String formatarTempo(float minutos) {
  int h = (int)minutos / 60;
  int m = (int)minutos % 60;
  return String(h) + "h" + (m < 10 ? "0" : "") + String(m);
}

String nomeEstado(Estado e) {
  switch (e) {
    case NORMAL:      return "VENDA NORMAL";
    case GELO:        return "COMPLETE O GELO";
    case XEPA:        return "HORA DA XEPA";
    case DESCARTE:    return "DESCARTE";
    case SEM_LEITURA: return "SEM LEITURA";
  }
  return "";
}

// "Nova carga": o feirante aperta quando coloca produto novo e a conta zera
void verificarBotao(unsigned long agora) {
  if (digitalRead(BOTAO) == LOW && agora - tBotao > 300) {
    tBotao = agora;
    minutosFora = 0;
    qtdLeituras = posLeitura = 0;
    qtdHist = posHist = 0;
    taxa = 0;
    bipar(1, 1500, 100);
    Serial.println(">> Nova carga, contagem zerada");
  }
}

// field1 temp, field2 minutos fora, field3 estado (0 a 4), field4 taxa, field5 minutos restantes
void enviarThingSpeak() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sem wifi, seguindo offline");
    return;
  }

  String url = "http://api.thingspeak.com/update?api_key=" + String(THINGSPEAK_KEY);
  url += "&field1=" + String(media, 1);
  url += "&field2=" + String((int)minutosFora);
  url += "&field3=" + String((int)estado);
  url += "&field4=" + String(taxa, 1);
  url += "&field5=" + String(minutosRestantes());

  HTTPClient http;
  http.begin(url);
  int codigo = http.GET();
  http.end();

  if (codigo == 200) {
    Serial.println("ThingSpeak: enviado");
  } else {
    Serial.print("ThingSpeak: falhou (");
    Serial.print(codigo);
    Serial.println(")");
  }
}
