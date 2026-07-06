#include <ESP32Servo.h>

// Definição dos pinos
#define POT_PIN 34    // Pino analógico ligado ao potenciômetro
#define SERVO_PIN 18  // Pino PWM ligado ao servomotor

Servo meuServo;

void setup() {
  Serial.begin(115200);
  
  // Associa o objeto do servo ao pino correspondente
  meuServo.attach(SERVO_PIN);
  
  Serial.println("Iniciando controle do Servomotor pelo ADC...");
}

void loop() {
  // 1. Lê o valor analógico do potenciômetro
  // O ADC da ESP32 tem resolução de 12 bits, gerando valores de 0 a 4095
  int adcValue = analogRead(POT_PIN);
  
  // 2. Converte (mapeia) o valor do ADC (0-4095) para o ângulo do servo (0-180 graus)
  int angulo = map(adcValue, 0, 4095, 0, 180);
  
  // 3. Converte o valor do ADC para a porcentagem do duty cycle (0-100%) para exibição
  int dutyCyclePercent = map(adcValue, 0, 4095, 0, 100);
  
  // 4. Envia o comando de posição para o servomotor
  meuServo.write(angulo);
  
  // 5. Imprime os resultados no monitor serial
  Serial.print("ADC: ");
  Serial.print(adcValue);
  Serial.print(" | Angulo: ");
  Serial.print(angulo);
  Serial.print(" graus | Duty Cycle: ");
  Serial.print(dutyCyclePercent);
  Serial.println("%");
  
  delay(50); // Pequeno atraso para estabilidade da simulação
}
