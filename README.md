# Projeto 3: Controle PWM e Comunicação
**Disciplina:** SEL0433 - Aplicação de Microprocessadores  
**Autores:** Pedro da Silva Panini nºusp:15483543; Lucas Gomes Pacheco nºusp:15697253; Carlos Eduardo Cintra Siqueira Rodrigues de Mattos nºusp:15445279.

## Introdução
Este projeto consiste no desenvolvimento de aplicações práticas baseadas na plataforma ESP32 DevKit, explorando o controle de periféricos por Modulação por Largura de Pulso (PWM), conversão analógico-digital (ADC) e comunicação de dados (UART, I2C, Wi-Fi). O sistema foi dividido em três frentes: controle automatizado de intensidade luminosa de um LED RGB (Parte 1), acionamento analógico de um servomotor (Parte 2 - Ex 1) e o monitoramento remoto do acionamento de um motor de passo com MCPWM (Parte 2 - Ex 2). O desenvolvimento e a validação do sistema foram realizados com o auxílio do ambiente de simulação virtual Wokwi.

## Funcionalidades Implementadas
- **Controle de LED RGB (Parte 1):** Utilização da API nativa ledcAttach da ESP-IDF (Core 3.0+) para controle de canais PWM independentes a 5 kHz e 8 bits de resolução. Taxas de incremento distintas e autônomas para cada cor (Verde = 5, Azul = 10, Vermelho = 15).
- **Acionamento Analógico de Servomotor (Parte 2 - Ex 1):** Integração do ADC de 12 bits para leitura de potenciômetro, com mapeamento (map) para conversão da tensão em modulação angular (0 a 180 graus) operando a 50 Hz via biblioteca ESP32_Servo.h.
- **Motor de Passo com MCPWM (Parte 2 - Ex 2):** Controle de velocidade e sentido do atuador utilizando os registradores avançados do ESP32 (mcpwm.h). Implementação de botão com interrupção para inversão de giro e filtro de histerese no potenciômetro para evitar jitter.
- **Monitoramento e Interface (Parte 2 - Ex 2):** Exibição das variáveis em tempo real em um display OLED via I2C e implementação de um Web Server via Wi-Fi na porta 80, permitindo a visualização remota dos parâmetros com auto-refresh.
- **Diretivas de Fallback:** Uso de diretivas não-bloqueantes via #if !MODO_SIMULADOR para garantir a compatibilidade do código avançado com as limitações dos componentes virtuais do Wokwi (substituindo o MCPWM por tone()).

## Esquemático e Hardware
As conexões e a arquitetura lógica do sistema foram validadas no Wokwi:
- **LED RGB (Parte 1):** Pinos GPIO 25 (Red), 26 (Green) e 27 (Blue) gerenciando os canais PWM.
- **Servomotor (Parte 2 - Ex 1):** Pino 34 configurado como entrada analógica (ADC) e Pino 18 dedicado ao sinal PWM do motor.
- **Sistema MCPWM (Parte 2 - Ex 2):** Pino 34 (Potenciômetro), Pino 25 (Botão com resistor de pull-up interno), Pino 14 (Sinal de Step/PWM) e Pino 26 (Direção do Motor).
- **Display OLED:** Barramento de dados I2C padrão.

## Simulações e Códigos do Projeto

### 1. Parte 1: Controle PWM de LED RGB

**Link da Simulação:**
(https://wokwi.com/projects/468826621772493825)

**Diagrama do Circuito:**
<img width="570" height="754" alt="image" src="https://github.com/user-attachments/assets/beef2ed4-160a-4477-9a57-4bad0a160c9f" />


**Trecho de Código em Destaque:**
```c
void setup() {
  Serial.begin(115200);     
  // Configuração com a NOVA API da biblioteca LEDC (Core 3.0+)
  ledcAttach(RED_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(GREEN_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(BLUE_PIN, PWM_FREQ, PWM_RESOLUTION);
}

void loop() {
  ledcWrite(RED_PIN, dutyRed);
  ledcWrite(GREEN_PIN, dutyGreen);
  ledcWrite(BLUE_PIN, dutyBlue);

  dutyRed += incRed;
  dutyGreen += incGreen;
  dutyBlue += incBlue;

  // Estruturas de decisão para inverter o passo ao atingir limites
  if (dutyRed >= 255 || dutyRed <= 0) incRed = -incRed;
  if (dutyGreen >= 255 || dutyGreen <= 0) incGreen = -incGreen;
  if (dutyBlue >= 255 || dutyBlue <= 0) incBlue = -incBlue;
  delay(50); 
}
```

### 2. Parte 2 (Exercício 1): Controle de Servomotor via ADC

**Link da Simulação:**
(https://wokwi.com/projects/468827570264113153)

**Diagrama do Circuito:**
<img width="669" height="471" alt="image" src="https://github.com/user-attachments/assets/df67a2ba-b3e9-403f-8db1-b26ae91e15ad" />


**Trecho de Código em Destaque:**
```c
void loop() {
  // O ADC da ESP32 tem resolução de 12 bits (0 a 4095)
  int adcValue = analogRead(POT_PIN);
  
  // Mapeia o valor do ADC (0-4095) para o ângulo do servo (0-180 graus)
  int angulo = map(adcValue, 0, 4095, 0, 180);
  int dutyCyclePercent = map(adcValue, 0, 4095, 0, 100);
  
  meuServo.write(angulo);
  delay(50);
}
```

### 3. Parte 2 (Exercício 2): Motor de Passo com MCPWM e Wi-Fi

**Link da Simulação:**
https://wokwi.com/projects/468828153633123329

**Diagrama do Circuito:**
<img width="512" height="570" alt="image" src="https://github.com/user-attachments/assets/590b22b4-fdb3-4e56-a1ad-67f4bbbd21a2" />


**Trecho de Código em Destaque:**
```c
  // CONFIGURAÇÃO DO GERADOR DE PULSOS
  #if !MODO_SIMULADOR
    // Implementação Física: Biblioteca Nativa MCPWM
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, frequenciaAtual);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 50.0);
  #else
    // Implementação Wokwi: Tone nativo compatível com o simulador visual
    tone(PIN_STEP, frequenciaAtual);
  #endif
```

---

## Discussão de Resultados e Reflexão Final

Ao longo deste projeto, foi possível observar na prática a robustez da ESP32 no gerenciamento assíncrono de múltiplos periféricos (geração de PWM, servidor web de forma simultânea e comunicação I2C). Os resultados simulados demonstraram estabilidade, especialmente após o tratamento digital de ruídos (histerese no ADC e debounce na ISR).

Avaliando o uso de Microcontroladores de 32 bits:
Durante a implementação, levantou-se a reflexão sobre quando é realmente justificado o uso de um microcontrolador tão robusto quanto a ESP32. Para aplicações simples, como o acionamento de um servomotor da Parte 2.1, chips de 8 bits seriam suficientes e representariam um custo financeiro e energético significativamente menor. 

Entretanto, para a Parte 2.2, o uso da ESP32 se torna vantajoso e necessário. A necessidade de operar uma pilha de rede (TCP/IP) para o Web Server simultaneamente com as temporizações críticas para a geração de pulsos de motor, interrupções externas e barramento I2C, inviabilizaria plataformas menos potentes. O processamento da ESP32 e os periféricos avançados como o MCPWM entregam a performance necessária em aplicações conectadas sem comprometer a estabilidade das lógicas de controle determinístico.
