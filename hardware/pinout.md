# Pinagem e conexões elétricas

Este documento registra as conexões utilizadas na implementação física da Prática 01.

| Componente | Pino do componente | Conexão | Tensão | Função | Cuidado |
|---|---|---|---:|---|---|
| Potenciômetro | Um terminal lateral | 3V3 | 3,3 V | Alimentação | Não conectar a 5 V |
| Potenciômetro | Terminal central | GPIO15 | 0–3,3 V | Sinal analógico para o ADC | A tensão deve permanecer dentro dos limites do ESP32 |
| Potenciômetro | Outro terminal lateral | GND | 0 V | Referência elétrica | Manter terra comum |
| LED verde | Ânodo | GPIO5 por meio de resistor de 220 Ω | Nível lógico do GPIO | Indicação do estado verde | Não ligar diretamente ao GPIO sem o resistor |
| LED amarelo | Ânodo | GPIO18 por meio de resistor de 220 Ω | Nível lógico do GPIO | Indicação do estado amarelo | Não ligar diretamente ao GPIO sem o resistor |
| LED vermelho | Ânodo | GPIO19 por meio de resistor de 220 Ω | Nível lógico do GPIO | Indicação do estado vermelho | Não ligar diretamente ao GPIO sem o resistor |
| LEDs | Cátodos | GND | 0 V | Retorno de corrente | Confirmar polaridade |
| OLED | VDD | 3V3 | 3,3 V | Alimentação do display | Usar a alimentação de 3,3 V da montagem |
| OLED | GND | GND | 0 V | Referência elétrica | Manter terra comum |
| OLED | SDA | GPIO21 | 3,3 V lógico | Dados I²C | Não confundir com SCL |
| OLED | SCK (SCL no código) | GPIO22 | 3,3 V lógico | Clock I²C | Não confundir com SDA |

## Ligações resumidas

```text
Potenciômetro: 3V3 ─ terminal lateral
               GPIO15 ─ terminal central
               GND ─ outro terminal lateral

LED verde:    GPIO5 ─ resistor 220 Ω ─ LED ─ GND
LED amarelo:  GPIO18 ─ resistor 220 Ω ─ LED ─ GND
LED vermelho: GPIO19 ─ resistor 220 Ω ─ LED ─ GND

OLED: 3V3 ─ VDD | GND ─ GND | GPIO21 ─ SDA | GPIO22 ─ SCK
```

O resistor pode ser colocado antes ou depois do LED no mesmo ramo elétrico; o ponto essencial é que permaneça **em série** para limitar a corrente.

## Energia, sinal e dados

- **Energia:** o cabo USB alimenta o ESP32; os pinos 3V3 e GND distribuem energia aos componentes.
- **Sinal elétrico:** o potenciômetro produz uma tensão variável entre GND e 3,3 V; os GPIOs dos LEDs controlam níveis lógicos de saída.
- **Dados:** SDA e SCL transportam a comunicação digital I²C entre o ESP32 e o OLED.

## Critérios de verificação

1. Com o circuito desligado, conferir a continuidade das conexões e a polaridade dos LEDs.
2. Confirmar que nenhum GPIO do ESP32 recebe sinal de 5 V.
3. Energizar e verificar no Monitor Serial se o OLED foi encontrado no endereço `0x3C` ou `0x3D`.
4. Girar o potenciômetro e observar a variação de ADC e tensão.
5. Conferir se somente o LED correspondente ao estado atual permanece aceso.
6. Comparar OLED e monitor serial para confirmar a coerência das informações.

Os valores `0x3C` e `0x3D` não representam tensão ou energia. Eles são possíveis endereços do OLED no barramento I²C; o prefixo `0x` indica que o número está escrito em hexadecimal.
