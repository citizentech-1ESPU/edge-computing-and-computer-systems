# Caixa térmica · Xepa Certa

Circuito da caixa térmica da Xepa Certa, simulado no Wokwi. Um ESP32 mede a temperatura dentro da caixa, soma o tempo que o produto passou acima de 5 °C e decide sozinho, mesmo sem internet, se o peixe ou o queijo pode ser vendido normal, se é hora de completar o gelo, se é hora da xepa ou se precisa ir para o descarte. Com wifi, manda as leituras para o ThingSpeak.

GS CitizenTech 2026.2 · Engenharia de Software, 1º ano · Edge Computing & Computer Systems · time SafeTech

- Simulação no Wokwi: https://wokwi.com/projects/480215731510086657
- Vídeo explicativo: https://youtu.be/4kJYAYRe5JI
- Canal público no ThingSpeak: https://thingspeak.mathworks.com/channels/3571880

Integrantes:

- Pedro Thyago de Araújo Santos, RM 570939
- Pedro Passos Corsini, RM 573493
- Daniel Gomes Torres, RM 573436
- Henrique Lira, RM 571009

## Ficha da CitizenTech

- **Nome:** Xepa Certa
- **Cidade e bairro:** São Paulo, Zona Norte, feira de domingo da Av. Engenheiro Caetano Álvares
- **Cidadão:** o feirante de perecível (peixe, queijo e frios), os fregueses do bairro e as ONGs e cozinhas solidárias da região
- **Problema em uma frase:** o feirante não tem como saber se o peixe e o queijo ficaram frios o suficiente, perde mercadoria quando o gelo acaba antes do esperado, e a sobra boa não chega a tempo a quem precisa.
- **Como funciona:** um sensor na caixa térmica mede a temperatura e soma o tempo fora do frio. A caixa mostra verde, amarelo ou vermelho mesmo sem internet. No amarelo, o feirante anuncia a xepa no site e fregueses e ONGs reservam. No vermelho, o produto sai da venda.
- **Indicador:** quilos de perecível que deixaram de ir para o lixo por feira (vendidos na xepa ou doados) e reais de perda evitada por feirante.
- **ODS:** 2 (meta 2.1, acesso a alimento), 3 (metas 3.3 e 3.d, saúde e alerta de risco) e 11 (meta 11.6, menos resíduo na cidade).

O que cada matéria entrega:

- Edge Computing: a caixa térmica com sensor ([repositório](https://github.com/citizentech-1ESPU/edge-computing-and-computer-systems))
- Front-end e Web Development: o site da xepa ([repositório](https://github.com/citizentech-1ESPU/web_development))
- Python: o painel da banca ([repositório](https://github.com/citizentech-1ESPU/computational-thinking-with-python))
- Matemática: o modelo de quanto o gelo dura ([repositório](https://github.com/citizentech-1ESPU/differentiated-problem-solving))
- STED: requisitos, jornada e protótipo ([repositório](https://github.com/citizentech-1ESPU/software-and-total-experience-design))
- Pitch: a história do projeto em 3 minutos

## O problema

Peixe e queijo ficam em caixas térmicas com gelo durante a feira. Em dia quente o gelo acaba antes do esperado, e o feirante só percebe quando o produto já esquentou. Como não tem como saber quanto tempo o peixe ficou fora do frio, ele joga fora tudo o que deixa dúvida. A caixa resolve isso medindo e decidindo no próprio lugar, onde a internet da feira não chega bem.

## Componentes e ligações

- ESP32 DevKitC
- Sensor de temperatura DS18B20 no pino 4, com resistor de 4,7 kΩ entre o sinal e o 3,3 V
- LED verde no pino 27, LED amarelo no 26 e LED vermelho no 25, cada um com resistor de 220 Ω
- Buzzer no pino 2
- Display LCD 16x2 com módulo I2C (endereço 0x27): SDA no pino 22 e SCL no pino 21
- Botão "Nova carga" no pino 13, com o pull-up interno do ESP32

## Como funciona

A regra vem da agência de alimentos da Austrália e Nova Zelândia (regra das 2 horas e 4 horas):

- até 2 horas acima de 5 °C: **venda normal** (LED verde);
- de 2 a 4 horas: **hora da xepa**, vender ou doar no dia (LED amarelo, dois bipes);
- a partir de 4 horas: **descarte** (LED vermelho, três bipes que se repetem).

Além disso:

- **Complete o gelo:** pela velocidade com que a caixa esquenta, ela avisa quando vai passar de 5 °C em até 15 minutos (LED verde piscando, um bipe).
- **Sem leitura:** leituras impossíveis são ignoradas. Com 3 seguidas, os três LEDs piscam juntos e o display pede para conferir o sensor.
- O tempo fora do frio nunca diminui: se a caixa esfria de novo, a conta continua de onde parou. Só zera com o botão "Nova carga".
- As leituras passam por uma média das últimas 5, para evitar alarme falso por ruído.
- Tudo roda com `millis()`, sem `delay()`: a caixa lê, avisa e envia dados ao mesmo tempo.

Para caber no vídeo, a simulação está acelerada: `FATOR_TEMPO = 120`, ou seja, cada segundo vale 2 minutos. Na caixa de verdade esse valor seria 1.

## ThingSpeak

A cada 20 segundos, quando tem wifi, a caixa envia para o canal 3571880:

- field1: temperatura (média, em °C)
- field2: minutos fora do frio
- field3: estado (0 normal, 1 gelo, 2 xepa, 3 descarte, 4 sem leitura)
- field4: velocidade de aquecimento (°C por hora)
- field5: minutos que faltam para o descarte

O site e o painel da banca leem esse canal pelo número, sem chave, porque ele é público.

## Como rodar

1. Abra o link do Wokwi e aperte o play.
2. Clique no sensor DS18B20 para aparecer o controle de temperatura e mude o valor para ver os estados.
3. O botão azul do circuito é o "Nova carga".

Neste repositório, a chave de escrita do ThingSpeak está como `COLOQUE_SUA_WRITE_API_KEY`. Para enviar dados para outro canal, troque pela chave dele no `sketch.ino`.

## Arquivos

- `sketch.ino`: código do ESP32
- `diagram.json`: circuito do Wokwi
- `libraries.txt`: bibliotecas usadas (OneWire, DallasTemperature, LiquidCrystal I2C)
- `prompts.md`: prompts usados e como a IA ajudou

## Uso de IA

Os prompts usados, o que a IA ajudou a fazer, como conferimos e o que ela errou estão no `prompts.md`.
