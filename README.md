# Prática 01 - Entrada Analógica com ESP32

Este projeto apresenta a montagem física de um sistema de entrada analógica com ESP32. Um potenciômetro gera uma tensão variável, o ESP32 converte essa tensão em um valor digital e indica a faixa medida por meio de três LEDs, um display OLED e o Monitor Serial.

[![Circuito completo com ESP32, potenciômetro, LEDs e OLED](assets/images/11-circuito-completo-com-oled.jpeg)](assets/images/11-circuito-completo-com-oled.jpeg)

> **Status do projeto:** montagem, firmware, testes e relatório acadêmico concluídos. Clique em qualquer imagem para abri-la em tamanho completo.

## Objetivos

- compreender a leitura de um sinal analógico no ESP32;
- utilizar o potenciômetro como divisor de tensão entre 0 V e 3,3 V;
- converter a leitura de 12 bits do ADC, de 0 a 4095, em tensão;
- controlar saídas digitais de acordo com a faixa medida;
- acompanhar o funcionamento pelos LEDs, pelo OLED e pelo Monitor Serial.

## Visão geral do funcionamento

```text
giro do potenciômetro
        ↓
tensão variável entre 0 V e 3,3 V
        ↓
GPIO15 e ADC de 12 bits do ESP32
        ↓
média de 10 leituras + máquina de estados com histerese
        ↓
LED correspondente + OLED + Monitor Serial
```

O firmware realiza dez leituras sucessivas e usa a média para reduzir pequenas oscilações. A conversão didática utilizada é:

```text
tensão (V) = leitura ADC × 3,3 / 4095
```

Essa fórmula é adequada para a prática, mas não substitui uma calibração do ADC para medições de laboratório.

## Componentes utilizados

- placa ESP32;
- protoboard;
- potenciômetro;
- LEDs verde, amarelo e vermelho;
- três resistores de **220 Ω**, um em série com cada LED;
- display OLED 128 × 64 com comunicação I²C;
- jumpers;
- cabo USB para alimentação e programação.

## Pinagem da montagem física

| Componente | Conexão no ESP32 | Função |
|---|---:|---|
| Terminal central do potenciômetro | GPIO15 | Entrada analógica |
| LED verde | GPIO5, com resistor de 220 Ω | Saída digital |
| LED amarelo | GPIO18, com resistor de 220 Ω | Saída digital |
| LED vermelho | GPIO19, com resistor de 220 Ω | Saída digital |
| OLED SDA | GPIO21 | Dados I²C |
| OLED SCK | GPIO22, identificado como SCL no código | Clock I²C |
| Alimentação | 3V3 | Energia em 3,3 V |
| Referência elétrica | GND | Terra comum |

As ligações detalhadas, os cuidados elétricos e a diferença entre energia, sinal e dados estão em [`hardware/pinout.md`](hardware/pinout.md).

## Estados e histerese

A histerese utiliza limites diferentes quando a tensão aumenta e quando diminui. Isso evita que o sistema troque rapidamente de LED devido a pequenas oscilações próximas aos limites.

| Transição | Aumentando a tensão | Diminuindo a tensão |
|---|---:|---:|
| Apagado ↔ verde | entra em verde acima de 0,55 V | volta a apagado abaixo de 0,45 V |
| Verde ↔ amarelo | entra em amarelo acima de 1,55 V | volta a verde abaixo de 1,45 V |
| Amarelo ↔ vermelho | entra em vermelho acima de 2,05 V | volta a amarelo abaixo de 1,95 V |

Dentro de cada intervalo de histerese, o estado anterior é mantido.

## Montagem passo a passo

### 1. Separação dos componentes

[![Componentes utilizados](assets/images/01-componentes.jpeg)](assets/images/01-componentes.jpeg)

*Figura 1 - Componentes separados antes da montagem.*

### 2. Preparação da protoboard

[![Protoboard vazia](assets/images/02-protoboard-vazia.jpeg)](assets/images/02-protoboard-vazia.jpeg)

*Figura 2 - Protoboard antes do posicionamento dos componentes.*

### 3. Posicionamento do ESP32

[![ESP32 na protoboard](assets/images/03-esp32-na-protoboard.jpeg)](assets/images/03-esp32-na-protoboard.jpeg)

*Figura 3 - ESP32 posicionado sobre o canal central da protoboard.*

### 4. Distribuição da alimentação

[![Alimentação 3V3 e GND](assets/images/04-alimentacao-3v3-gnd.jpeg)](assets/images/04-alimentacao-3v3-gnd.jpeg)

*Figura 4 - Distribuição de 3,3 V e GND para alimentação e terra comum.*

### 5. Posicionamento do potenciômetro

[![Potenciômetro posicionado](assets/images/05-potenciometro-posicionado.jpeg)](assets/images/05-potenciometro-posicionado.jpeg)

*Figura 5 - Potenciômetro inserido na protoboard antes das conexões.*

### 6. Conexão da entrada analógica

[![Potenciômetro conectado](assets/images/06-potenciometro-conectado.jpeg)](assets/images/06-potenciometro-conectado.jpeg)

*Figura 6 - Terminais laterais ligados a 3V3 e GND e terminal central ligado ao GPIO15.*

### 7. Posicionamento dos LEDs

[![LEDs posicionados](assets/images/07-leds-posicionados.jpeg)](assets/images/07-leds-posicionados.jpeg)

*Figura 7 - LEDs verde, amarelo e vermelho posicionados na protoboard.*

### 8. Inclusão dos resistores

[![LEDs com resistores](assets/images/08-leds-com-resistores.jpeg)](assets/images/08-leds-com-resistores.jpeg)

*Figura 8 - Resistores de 220 Ω adicionados em série com os LEDs.*

### 9. Conexões dos GPIOs

[![Conexões dos GPIOs dos LEDs](assets/images/09-conexoes-gpio-dos-leds.jpeg)](assets/images/09-conexoes-gpio-dos-leds.jpeg)

*Figura 9 - LEDs associados aos GPIOs 5, 18 e 19.*

### 10. Integração do OLED

[![OLED com cabos](assets/images/10-oled-com-cabos.jpeg)](assets/images/10-oled-com-cabos.jpeg)

*Figura 10 - Display OLED preparado para alimentação e comunicação I²C.*

### 11. Circuito completo

[![Circuito completo com OLED](assets/images/11-circuito-completo-com-oled.jpeg)](assets/images/11-circuito-completo-com-oled.jpeg)

*Figura 11 - Protótipo físico completo.*

## Testes realizados

As fotografias comprovam o acionamento físico dos LEDs e as capturas do Monitor Serial registram ADC, tensão calculada e estado lógico. Os valores abaixo são pontos observados durante os testes.

| Estado | ADC observado | Tensão observada | Resultado |
|---|---:|---:|---|
| Apagado | 0 | 0,000 V | Todos os LEDs apagados |
| Verde | aproximadamente 663 | aproximadamente 0,534 V | LED verde aceso |
| Amarelo | 1927–1928 | 1,553–1,554 V | LED amarelo aceso |
| Vermelho | 2559 | 2,062 V | LED vermelho aceso |

O registro do verde em 0,534 V demonstra a manutenção do estado dentro da faixa de histerese; o limite programado para entrar no verde durante a subida é superior a 0,55 V.

| Circuito físico | Monitor Serial |
|---|---|
| [![Circuito no estado apagado](assets/images/testes/12a-teste-apagado-circuito.jpeg)](assets/images/testes/12a-teste-apagado-circuito.jpeg) | [![Monitor Serial no estado apagado](assets/images/testes/12b-teste-apagado-monitor-serial.png)](assets/images/testes/12b-teste-apagado-monitor-serial.png) |
| [![LED verde aceso](assets/images/testes/13a-teste-led-verde-circuito.jpeg)](assets/images/testes/13a-teste-led-verde-circuito.jpeg) | [![Monitor Serial no estado verde](assets/images/testes/13b-teste-led-verde-monitor-serial.png)](assets/images/testes/13b-teste-led-verde-monitor-serial.png) |
| [![LED amarelo aceso](assets/images/testes/14a-teste-led-amarelo-circuito.jpeg)](assets/images/testes/14a-teste-led-amarelo-circuito.jpeg) | [![Monitor Serial no estado amarelo](assets/images/testes/14b-teste-led-amarelo-monitor-serial.png)](assets/images/testes/14b-teste-led-amarelo-monitor-serial.png) |
| [![LED vermelho aceso](assets/images/testes/15a-teste-led-vermelho-circuito.jpeg)](assets/images/testes/15a-teste-led-vermelho-circuito.jpeg) | [![Monitor Serial no estado vermelho](assets/images/testes/15b-teste-led-vermelho-monitor-serial.png)](assets/images/testes/15b-teste-led-vermelho-monitor-serial.png) |

## Vídeos

- [Demonstração do circuito físico](media/demonstracao-pratica-01.mp4)
- [Teste das transições no Monitor Serial](media/teste-transicoes-monitor-serial.mp4)

Os vídeos são evidências complementares. As fotografias e capturas acima mantêm os principais resultados visíveis diretamente no GitHub e no relatório.

## Firmware

O código completo está em [`firmware/pratica01-entrada-analogica.ino`](firmware/pratica01-entrada-analogica.ino).

### Recursos implementados

- ADC configurado com resolução de 12 bits;
- média de 10 amostras;
- máquina de estados com histerese;
- três saídas digitais para os LEDs;
- OLED com leitura ADC, tensão e estado;
- detecção automática do OLED nos endereços I²C `0x3C` e `0x3D`;
- saída textual no Monitor Serial a 115200 bit/s.

### Bibliotecas necessárias

- `Wire`, incluída no suporte do ESP32;
- `Adafruit GFX Library`;
- `Adafruit SSD1306`.

### Como executar

1. Abra o arquivo `.ino` na Arduino IDE.
2. Instale as duas bibliotecas da Adafruit, caso ainda não estejam instaladas.
3. Selecione a placa ESP32 e a porta USB correta.
4. Compile e envie o firmware.
5. Abra o Monitor Serial em 115200 bit/s.
6. Gire o potenciômetro e compare o LED aceso, o OLED e o Monitor Serial.

## Relatório acadêmico

- [`Abrir ou baixar o relatório final em PDF`](docs/relatorio-pratica-01-entrada-analogica-esp32.pdf?raw=1)


## Cuidados elétricos

- utilizar 3,3 V nos sinais conectados diretamente aos GPIOs do ESP32;
- manter GND comum entre todos os componentes;
- não conectar LEDs sem resistores limitadores de corrente;
- conferir a polaridade dos LEDs e do OLED antes de energizar;
- desligar a alimentação antes de alterar a montagem.

## Autor

**Ítalo Dias Moreira Campos**  
Bacharelado em Inteligência Artificial - 4º período  
Disciplina: Internet das Coisas e Robótica  
Professor: Dr. Tassio Ferenzini Martins Sirqueira
