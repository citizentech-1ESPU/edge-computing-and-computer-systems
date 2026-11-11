# Uso de IA

Usamos o Claude (Anthropic) como apoio na parte de Edge Computing. Tudo o que ele sugeriu foi conferido por nós antes de entrar no projeto.

## Onde a IA ajudou

- **Código do ESP32:** pedimos ajuda para organizar o `sketch.ino` sem `delay()`, com `millis()`, e para montar a média das leituras e a conta da velocidade de aquecimento. As regras (5 °C, 2 horas e 4 horas, aviso de 15 minutos, 3 leituras inválidas) vieram do documento de design do time.
- **Circuito no Wokwi:** ajuda para escolher os pinos e montar o `diagram.json`.
- **ThingSpeak:** ajuda para enviar os cinco campos e para resolver o wifi que não conectava no Wokwi (era preciso passar o canal 6 no `WiFi.begin`).
- **Vídeo:** ajuda para escrever o roteiro da fala e para juntar a gravação da tela com a narração.

## Como conferimos

- Rodamos a simulação inteira no Wokwi, passando por todos os estados: venda normal, complete o gelo, hora da xepa, descarte, Nova carga e sem leitura.
- Conferimos no Serial e no display se o tempo fora do frio somava certo e não diminuía quando a caixa esfriava.
- Conferimos no ThingSpeak se os cinco campos chegavam na ordem combinada com o site e o painel.
- A fala do vídeo foi gravada por um integrante do time, com as palavras ajustadas ao nosso jeito de explicar.
