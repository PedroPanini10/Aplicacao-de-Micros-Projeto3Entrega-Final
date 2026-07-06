// CONFIGURAÇÕES DE HARDWARE (PINOS) E PARÂMETROS DO PWM
#define RED_PIN 25          // Pino de controle do LED Vermelho
#define GREEN_PIN 26        // Pino de controle do LED Verde
#define BLUE_PIN 27         // Pino de controle do LED Azul

#define PWM_FREQ 5000       // Frequência do sinal PWM em 5000 Hz (5 kHz)
#define PWM_RESOLUTION 8    // Resolução de 8 bits (duty cycle de 0 a 255)

// VARIÁVEIS DE CONTROLE DOS VALORES ATUAIS DE DUTY CYCLE
int dutyRed = 0;
int dutyGreen = 0;
int dutyBlue = 0;

// PASSO DE INCREMENTO/DECREMENTO PARA CADA COMPONENTE DE COR
int incGreen = 5;           // Passo base da cor verde
int incBlue = 10;           // Passo da cor azul (dobro do verde)
int incRed = 15;            // Passo da cor vermelha (triplo do verde)

void setup() {
  Serial.begin(115200);     

  // 1. Configuração direta com a NOVA API da biblioteca LEDC (Core 3.0+)
  // Sintaxe: ledcAttach(pino, frequencia, resolucao)
  ledcAttach(RED_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(GREEN_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(BLUE_PIN, PWM_FREQ, PWM_RESOLUTION);
  
  Serial.println("Iniciando controle PWM do LED RGB via nova API LEDC...");
}

void loop() {
  // 2. Escrita do duty cycle diretamente no PINO (o canal é gerenciado internamente)
  ledcWrite(RED_PIN, dutyRed);
  ledcWrite(GREEN_PIN, dutyGreen);
  ledcWrite(BLUE_PIN, dutyBlue);

  // Bloco de comandos Serial para formatar e exibir os dados
  Serial.print("Duty Cycles -> R: ");
  Serial.print(dutyRed);
  Serial.print(" (Inc: ");
  Serial.print(incRed);
  Serial.print(") | G: ");
  Serial.print(dutyGreen);
  Serial.print(" (Inc: ");
  Serial.print(incGreen);
  Serial.print(") | B: ");
  Serial.print(dutyBlue);
  Serial.print(" (Inc: ");
  Serial.print(incBlue);
  Serial.println(")");

  // Atualização matemática dos valores de intensidade
  dutyRed += incRed;
  dutyGreen += incGreen;
  dutyBlue += incBlue;

  // Estruturas de decisão para inverter o passo ao atingir os limites (0 ou 255)
  if (dutyRed >= 255 || dutyRed <= 0) incRed = -incRed;
  if (dutyGreen >= 255 || dutyGreen <= 0) incGreen = -incGreen;
  if (dutyBlue >= 255 || dutyBlue <= 0) incBlue = -incBlue;

  delay(50); 
}
