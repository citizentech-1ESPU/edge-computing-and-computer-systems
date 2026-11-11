# Fala do vídeo de Edge: Xepa Certa

Grave um arquivo de áudio para cada parte (6 no total).
Leia com calma, como se estivesse explicando para um colega. Pausas são boas: eu encaixo a imagem depois.
Tempo total de fala: perto de 5 minutos e meio.

---

## Parte 1. A ideia (cerca de 50 s)

Oi, eu sou o Pedro Thyago, do time SafeTech, e esse é o Xepa Certa.

O nosso cidadão é o feirante que vende peixe e queijo na feira de domingo, na Zona Norte de São Paulo. Numa entrevista, ele contou o problema: perto do meio-dia, o gelo da caixa térmica derrete, a caixa esquenta e ele só percebe tarde. Como não tem como saber quanto tempo o peixe ficou fora do frio, ele joga fora tudo o que deixa dúvida.

O próprio desafio da Global Solution cita a barraca da feira que precisa conservar alimentos como um lugar onde a tecnologia tem que funcionar sozinha. É isso que o nosso dispositivo faz. Ele fica dentro da caixa térmica, mede a temperatura e decide sozinho, mesmo sem internet, se o produto pode ser vendido normal, se é hora de completar o gelo, se é hora da xepa ou se precisa ir pro descarte.

O projeto atende três ODS: o 2, de acesso a alimento seguro; o 3, de evitar doença causada por alimento; e o 11, de diminuir o lixo nas cidades.

---

## Parte 2. O circuito (cerca de 1 min)

Agora o circuito.

O cérebro é um ESP32. A gente escolheu ele porque tem wifi e consegue mandar os dados pra nuvem quando tem sinal.

Esse é o sensor de temperatura DS18B20. Ele existe numa versão à prova d'água, que dá pra colocar no meio do gelo, junto do peixe.

Esses três LEDs mostram o estado de longe. Verde é venda normal, amarelo é hora da xepa e vermelho é descarte. Na feira, o feirante está de mãos ocupadas, então ele precisa entender só olhando a cor.

O buzzer avisa com som, e cada estado tem um bipe diferente.

O display mostra a temperatura, há quanto tempo o produto está fora do frio e o estado escrito por extenso.

E esse botão é o "Nova carga". Quando o feirante coloca produto novo na caixa, ele aperta e a contagem começa do zero.

---

## Parte 3. A lógica (cerca de 1 min e 10 s)

Agora a lógica.

A regra vem da agência de alimentos da Austrália e Nova Zelândia: a regra das 2 horas e 4 horas. O limite é 5 graus. A caixa soma o tempo que o produto passou acima disso. Até 2 horas, venda normal. De 2 a 4 horas, é hora da xepa: vender ou doar logo. Passou de 4 horas, descarte. Esses limites estão nessas constantes, no começo do código.

Pra caber no vídeo, a simulação está acelerada: cada segundo vale 2 minutos. Na caixa de verdade, esse fator seria 1.

A cada segundo, o código lê o sensor. Se a leitura for impossível, ele ignora. Se isso acontecer 3 vezes seguidas, a caixa entra em "sem leitura", porque um sensor com defeito não pode liberar nem condenar o produto.

Pra não dar alarme falso por causa de ruído, ele usa a média das últimas 5 leituras.

E aqui ele decide o estado. O tempo fora do frio nunca diminui: se a caixa esfria de novo, a conta continua de onde parou, porque repor o gelo não desfaz o que já esquentou. Ele também calcula a velocidade com que a caixa está esquentando e avisa pra completar o gelo antes de passar dos 5 graus.

Tudo roda sem travar: a caixa lê, avisa e manda os dados ao mesmo tempo.

---

## Parte 4. A simulação (cerca de 1 min e 40 s)

Agora a simulação funcionando.

A caixa começa bem gelada, em 2 graus. LED verde, e o display mostra "venda normal".

Agora a temperatura está subindo. Ainda não passou de 5 graus, mas pela velocidade a caixa vai passar logo. Então o verde começa a piscar, toca um bipe e aparece "complete o gelo". Era exatamente esse aviso que faltava pro feirante.

Agora a caixa passou de 5 graus. O aviso continua, e o display começa a contar o tempo fora do frio. Com a simulação acelerada, as horas passam rápido.

Chegou a 2 horas. LED amarelo, dois bipes e "hora da xepa". O produto ainda está bom, mas precisa ser vendido ou doado hoje.

Chegou a 4 horas. LED vermelho, três bipes que se repetem e "descarte". Esse produto não pode ser vendido nem doado.

Mesmo voltando a temperatura pra 2 graus, continua em descarte, porque o tempo que passou não volta.

Agora o feirante colocou produto novo e apertou "Nova carga". A contagem zerou e o verde voltou.

Por último, a falha. Coloquei um valor impossível, como se o sensor estivesse com defeito. As leituras são ignoradas e, na terceira seguida, os três LEDs piscam juntos e aparece "sem leitura, confira o sensor".

---

## Parte 5. A nuvem (cerca de 30 s)

Quando tem wifi, a caixa manda a cada 20 segundos, pro ThingSpeak, a temperatura, o tempo fora do frio, o estado e quanto tempo falta pro descarte. Nesses gráficos dá pra ver a simulação que vocês acabaram de assistir: a temperatura subindo, o tempo fora do frio crescendo e o estado indo de venda normal até descarte.

É daqui que o site do Xepa Certa puxa os dados pra mostrar as caixas e anunciar a xepa. Mas a decisão é sempre da caixa: sem internet, ela continua avisando do mesmo jeito.

---

## Encerramento (cerca de 10 s)

Esse foi o Xepa Certa, do time SafeTech: Pedro Thyago, Pedro Corsini, Daniel Gomes e Henrique Lira. Obrigado!
