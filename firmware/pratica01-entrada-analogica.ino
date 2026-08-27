#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// PRATICA 01 - ENTRADA ANALOGICA COM ESP32
// VERSAO APRIMORADA
//
// Recursos:
// - Potenciometro como entrada analogica
// - ADC de 12 bits
// - 3 LEDs como saidas digitais
// - Display OLED I2C
// - Media de leituras para reduzir ruido
// - Histerese para evitar troca de LED no limite
// =====================================================


// =====================================================
// 1. PINOS DA MONTAGEM
// =====================================================

// Potenciometro
const int PINO_POTENCIOMETRO = 15;

// LEDs
const int PINO_LED_VERDE = 5;
const int PINO_LED_AMARELO = 18;
const int PINO_LED_VERMELHO = 19;

// OLED I2C
const int PINO_SDA = 21;
const int PINO_SCL = 22;


// =====================================================
// 2. DISPLAY OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

bool display_funcionando = false;


// =====================================================
// 3. ADC E TENSAO
// =====================================================

// ADC de 12 bits:
// 2^12 = 4096 valores
// de 0 ate 4095
const int VALOR_MAXIMO_ADC = 4095;

// Tensao utilizada na pratica
const float TENSAO_REFERENCIA_V = 3.3;


// =====================================================
// 4. FILTRO DE MEDIA
// =====================================================
//
// Em vez de usar apenas uma leitura do ADC,
// fazemos varias leituras e calculamos a media.
//
// Isso reduz pequenas oscilacoes provocadas por ruido.
//

const int NUMERO_AMOSTRAS = 10;


// =====================================================
// 5. LIMITES ORIGINAIS DA PRATICA
// =====================================================
//
// Originalmente:
//
// ate 0,50 V       -> apagado
// 0,50 a 1,50 V    -> verde
// 1,50 a 2,00 V    -> amarelo
// acima de 2,00 V   -> vermelho
//
// A histerese cria uma pequena margem ao redor
// desses pontos.
// =====================================================


// -----------------------------------------------------
// APAGADO <-> VERDE
// -----------------------------------------------------

const float ENTRA_VERDE_V = 0.55;
const float VOLTA_APAGADO_V = 0.45;


// -----------------------------------------------------
// VERDE <-> AMARELO
// -----------------------------------------------------

const float ENTRA_AMARELO_V = 1.55;
const float VOLTA_VERDE_V = 1.45;


// -----------------------------------------------------
// AMARELO <-> VERMELHO
// -----------------------------------------------------

const float ENTRA_VERMELHO_V = 2.05;
const float VOLTA_AMARELO_V = 1.95;


// =====================================================
// 6. ESTADOS POSSIVEIS
// =====================================================

enum EstadoLed {
  APAGADO,
  VERDE,
  AMARELO,
  VERMELHO
};

EstadoLed estado_atual = APAGADO;


// =====================================================
// 7. VARIAVEIS DE MEDICAO
// =====================================================

int leitura_adc = 0;

float tensao_v = 0.0;


// =====================================================
// 8. LE O ADC FAZENDO MEDIA
// =====================================================

int lerAdcComMedia() {

  long soma = 0;

  for (int i = 0; i < NUMERO_AMOSTRAS; i++) {

    soma += analogRead(PINO_POTENCIOMETRO);

    delay(2);
  }

  return soma / NUMERO_AMOSTRAS;
}


// =====================================================
// 9. CONVERTE ADC PARA TENSAO
// =====================================================

float converterAdcParaTensao(int valor_adc) {

  return
    (valor_adc * TENSAO_REFERENCIA_V)
    / VALOR_MAXIMO_ADC;
}


// =====================================================
// 10. APAGA TODOS OS LEDs
// =====================================================

void apagarTodosOsLeds() {

  digitalWrite(PINO_LED_VERDE, LOW);
  digitalWrite(PINO_LED_AMARELO, LOW);
  digitalWrite(PINO_LED_VERMELHO, LOW);
}


// =====================================================
// 11. RETORNA O NOME DO ESTADO
// =====================================================

const char* nomeEstado() {

  switch (estado_atual) {

    case APAGADO:
      return "APAGADO";

    case VERDE:
      return "VERDE";

    case AMARELO:
      return "AMARELO";

    case VERMELHO:
      return "VERMELHO";
  }

  return "ERRO";
}


// =====================================================
// 12. CONTROLE COM HISTERESE
// =====================================================
//
// A histerese usa limites diferentes para subir
// e para descer.
//
// Exemplo:
//
// AMARELO -> VERMELHO:
// somente acima de 2,05 V
//
// VERMELHO -> AMARELO:
// somente abaixo de 1,95 V
//
// Entre 1,95 e 2,05 V:
// mantem o estado anterior.
// =====================================================

void atualizarEstadoComHisterese() {

  switch (estado_atual) {


    // -------------------------------------------------
    // ESTADO APAGADO
    // -------------------------------------------------

    case APAGADO:

      // Se o potenciometro ja estiver alto,
      // permite ir diretamente para o estado correto.

      if (tensao_v > ENTRA_VERMELHO_V) {

        estado_atual = VERMELHO;
      }

      else if (tensao_v > ENTRA_AMARELO_V) {

        estado_atual = AMARELO;
      }

      else if (tensao_v > ENTRA_VERDE_V) {

        estado_atual = VERDE;
      }

      break;


    // -------------------------------------------------
    // ESTADO VERDE
    // -------------------------------------------------

    case VERDE:

      if (tensao_v > ENTRA_VERMELHO_V) {

        estado_atual = VERMELHO;
      }

      else if (tensao_v > ENTRA_AMARELO_V) {

        estado_atual = AMARELO;
      }

      else if (tensao_v < VOLTA_APAGADO_V) {

        estado_atual = APAGADO;
      }

      break;


    // -------------------------------------------------
    // ESTADO AMARELO
    // -------------------------------------------------

    case AMARELO:

      if (tensao_v > ENTRA_VERMELHO_V) {

        estado_atual = VERMELHO;
      }

      else if (tensao_v < VOLTA_APAGADO_V) {

        estado_atual = APAGADO;
      }

      else if (tensao_v < VOLTA_VERDE_V) {

        estado_atual = VERDE;
      }

      break;


    // -------------------------------------------------
    // ESTADO VERMELHO
    // -------------------------------------------------

    case VERMELHO:

      // Se cair bastante, tambem permite ir
      // diretamente para estados inferiores.

      if (tensao_v < VOLTA_APAGADO_V) {

        estado_atual = APAGADO;
      }

      else if (tensao_v < VOLTA_VERDE_V) {

        estado_atual = VERDE;
      }

      else if (tensao_v < VOLTA_AMARELO_V) {

        estado_atual = AMARELO;
      }

      break;
  }
}


// =====================================================
// 13. ACENDE O LED CORRESPONDENTE
// =====================================================

void atualizarLeds() {

  apagarTodosOsLeds();

  switch (estado_atual) {

    case APAGADO:

      // Todos permanecem apagados.
      break;


    case VERDE:

      digitalWrite(
        PINO_LED_VERDE,
        HIGH
      );

      break;


    case AMARELO:

      digitalWrite(
        PINO_LED_AMARELO,
        HIGH
      );

      break;


    case VERMELHO:

      digitalWrite(
        PINO_LED_VERMELHO,
        HIGH
      );

      break;
  }
}


// =====================================================
// 14. PROCURA O OLED
// =====================================================

byte procurarOLED() {

  byte enderecos[] = {
    0x3C,
    0x3D
  };

  for (int i = 0; i < 2; i++) {

    Wire.beginTransmission(
      enderecos[i]
    );

    byte erro =
      Wire.endTransmission();

    if (erro == 0) {

      return enderecos[i];
    }
  }

  return 0;
}


// =====================================================
// 15. ATUALIZA O DISPLAY
// =====================================================

void atualizarDisplay() {

  if (!display_funcionando) {

    return;
  }

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setTextSize(1);


  // ---------------------------------------------------
  // TITULO
  // ---------------------------------------------------

  display.setCursor(24, 0);

  display.println(
    "PRATICA 01"
  );

  display.drawLine(
    0,
    10,
    127,
    10,
    SSD1306_WHITE
  );


  // ---------------------------------------------------
  // ADC
  // ---------------------------------------------------

  display.setCursor(0, 17);

  display.print("ADC: ");

  display.println(
    leitura_adc
  );


  // ---------------------------------------------------
  // TENSAO
  // ---------------------------------------------------

  display.setCursor(0, 31);

  display.print("Tensao: ");

  // Agora mostra 3 casas decimais.
  display.print(
    tensao_v,
    3
  );

  display.println(" V");


  // ---------------------------------------------------
  // LED
  // ---------------------------------------------------

  display.setCursor(0, 46);

  display.print("LED: ");

  display.println(
    nomeEstado()
  );


  display.display();
}


// =====================================================
// 16. MONITOR SERIAL
// =====================================================

void atualizarSerial() {

  Serial.print("ADC: ");

  Serial.print(
    leitura_adc
  );

  Serial.print(
    " | Tensao: "
  );

  Serial.print(
    tensao_v,
    3
  );

  Serial.print(
    " V | LED: "
  );

  Serial.println(
    nomeEstado()
  );
}


// =====================================================
// 17. SETUP
// =====================================================

void setup() {

  Serial.begin(115200);


  // ---------------------------------------------------
  // ADC
  // ---------------------------------------------------

  analogReadResolution(12);

  pinMode(
    PINO_POTENCIOMETRO,
    INPUT
  );


  // ---------------------------------------------------
  // LEDs
  // ---------------------------------------------------

  pinMode(
    PINO_LED_VERDE,
    OUTPUT
  );

  pinMode(
    PINO_LED_AMARELO,
    OUTPUT
  );

  pinMode(
    PINO_LED_VERMELHO,
    OUTPUT
  );

  apagarTodosOsLeds();


  // ---------------------------------------------------
  // I2C / OLED
  // ---------------------------------------------------

  Wire.begin(
    PINO_SDA,
    PINO_SCL
  );

  byte enderecoOLED =
    procurarOLED();


  if (enderecoOLED != 0) {

    Serial.print(
      "OLED encontrado em 0x"
    );

    Serial.println(
      enderecoOLED,
      HEX
    );


    display_funcionando =
      display.begin(
        SSD1306_SWITCHCAPVCC,
        enderecoOLED
      );


    if (display_funcionando) {

      display.clearDisplay();

      display.setTextColor(
        SSD1306_WHITE
      );

      display.setTextSize(1);


      display.setCursor(
        25,
        15
      );

      display.println(
        "PRATICA 01"
      );


      display.setCursor(
        8,
        35
      );

      display.println(
        "SISTEMA INICIADO"
      );


      display.display();

      delay(1500);
    }
  }

  else {

    Serial.println(
      "OLED nao encontrado."
    );

    Serial.println(
      "Os LEDs continuarao funcionando."
    );
  }
}


// =====================================================
// 18. LOOP PRINCIPAL
// =====================================================

void loop() {

  // ---------------------------------------------------
  // 1. LE O POTENCIOMETRO
  // ---------------------------------------------------

  leitura_adc =
    lerAdcComMedia();


  // ---------------------------------------------------
  // 2. CONVERTE PARA TENSAO
  // ---------------------------------------------------

  tensao_v =
    converterAdcParaTensao(
      leitura_adc
    );


  // ---------------------------------------------------
  // 3. DECIDE O ESTADO
  // ---------------------------------------------------

  atualizarEstadoComHisterese();


  // ---------------------------------------------------
  // 4. ATUALIZA AS SAIDAS
  // ---------------------------------------------------

  atualizarLeds();


  // ---------------------------------------------------
  // 5. MOSTRA NO OLED
  // ---------------------------------------------------

  atualizarDisplay();


  // ---------------------------------------------------
  // 6. MOSTRA NO MONITOR SERIAL
  // ---------------------------------------------------

  atualizarSerial();


  // ---------------------------------------------------
  // 7. PEQUENA PAUSA
  // ---------------------------------------------------

  delay(100);
}