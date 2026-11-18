# Uso de IA

## Ferramentas

- **Claude (Anthropic):** apoio para escrever e organizar o código do ESP32, montar o circuito no Wokwi, ligar a caixa ao ThingSpeak, escrever o roteiro do vídeo e juntar a gravação da tela com a narração.

## Prompts usados

Alguns dos pedidos feitos, com as nossas palavras:

- "Vamos começar com edge computing. A caixa precisa medir a temperatura, somar o tempo acima de 5 °C e avisar com LED, buzzer e display quando é hora do gelo, da xepa e do descarte, seguindo o documento de design."
- "O wifi não conecta no Wokwi. O que pode ser?"
- "Faz o código sem delay, só com millis, e com poucos comentários, só os necessários."
- "Testa no Wokwi se todos os estados estão funcionando: normal, gelo, xepa, descarte, Nova carga e sem leitura."
- "Faz o roteiro de fala do vídeo de Edge, mostrando a ideia, o circuito, o código e a simulação."
- "Na parte do sketch.ino, congela a tela enquanto eu falo, porque estou rolando o código e fica difícil de entender."

## O que foi feito com ajuda da IA

- **Código do ESP32 (`sketch.ino`):**
  - leitura do sensor sem travar o loop, com `millis()`;
  - média das últimas 5 leituras e descarte de leituras impossíveis;
  - velocidade de aquecimento e aviso de gelo 15 minutos antes de passar de 5 °C;
  - bipes diferentes para cada estado, sem `delay()`.
- **Circuito (`diagram.json`):** escolha dos pinos e ligação do display por I2C.
- **ThingSpeak:** envio dos cinco campos a cada 20 segundos e a correção do wifi, que só conectou passando o canal 6 no `WiFi.begin`.
- **Vídeo:** roteiro da fala e edição, encaixando cada trecho da narração no momento certo da simulação.

As regras da caixa (limite de 5 °C, 2 horas para a xepa, 4 horas para o descarte, 3 leituras inválidas) não vieram da IA. Elas vieram do documento de design do time, baseado na regra das 2 horas e 4 horas da agência de alimentos da Austrália e Nova Zelândia.

## Como conferimos

- Rodamos a simulação inteira no Wokwi pelo link público, sem estar logado, passando por todos os estados.
- Conferimos no Serial e no display se o tempo fora do frio somava 2 minutos por segundo e não diminuía quando a caixa esfriava.
- Testamos a falha do sensor com um valor impossível e vimos a caixa entrar em "sem leitura" exatamente na terceira leitura seguida.
- Conferimos no ThingSpeak se os cinco campos chegavam na ordem combinada com o site e o painel da banca.
- Assistimos o vídeo inteiro antes de publicar, conferindo se cada fala batia com o que aparecia na tela.

## Análise crítica

A IA ajudou bastante a organizar o código sem `delay()`, que era a parte mais difícil para nós, e a achar o problema do wifi, que sozinhos levaríamos mais tempo para descobrir. Também economizou tempo na edição do vídeo.

Mas ela não acertou tudo de primeira. O primeiro roteiro dizia que a xepa aparecia "em 1 minuto", e na nossa gravação levou de 2 a 3, porque a simulação roda mais devagar que o tempo real. Também foi preciso pedir para tirar comentários em excesso do código e para deixar os textos com o nosso jeito de falar.

A lição foi que a IA acelera o trabalho, mas cada coisa que ela gera precisa ser testada. Por isso rodamos a simulação várias vezes antes de gravar, e cada integrante consegue explicar como a caixa decide o estado.
