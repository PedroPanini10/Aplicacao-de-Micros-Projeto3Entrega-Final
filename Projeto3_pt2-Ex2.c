/*
 * Projeto: Controle de Motor de Passo com MCPWM
 * Descrição: Sistema em ambiente virtual de controle de motor com biblioteca 
 * nativa MCPWM, display OLED (I2C) e monitoramento remoto via Web Server (Wi-Fi).
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include "driver/mcpwm.h" 

// --- Seletor de Ambiente ---
// Criado para funcionar no wokwi.
#define MODO_SIMULADOR true 

// --- Definições de Pinos ---
#define PIN_POT    34 
#define PIN_BTN    25 
#define PIN_STEP   14 
#define PIN_DIR    26 
#define SCREEN_W   128
#define SCREEN_H   64

// --- Instâncias e Variáveis Globais ---
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
WebServer server(80);

volatile bool sentidoHorario = true;
volatile unsigned long ultimoDebounce = 0;
unsigned long ultimaAtualizacaoOled = 0;
int frequenciaAtual = 0;
int frequenciaAnterior = -50; 

// --- Interrupção do Botão (ISR) ---
void IRAM_ATTR inverteSentido() {
  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoDebounce > 250) { 
    sentidoHorario = !sentidoHorario;
    digitalWrite(PIN_DIR, sentidoHorario); 
    ultimoDebounce = tempoAtual;
  }
}

// --- Configuração do Wi-Fi e Web Server ---
void setupWiFi() {
  Serial.print("Conectando ao Wi-Fi Wokwi-GUEST...");
  WiFi.begin("Wokwi-GUEST", "", 6); 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado! IP para o Painel Web:");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
    html += "<title>Painel do Motor</title><style>";
    html += "body { font-family: Arial; text-align: center; margin-top: 50px; }";
    html += "h1 { color: #333; } .card { border: 1px solid #ccc; padding: 20px; display: inline-block; border-radius: 10px; }";
    html += "</style></head><meta http-equiv=\"refresh\" content=\"2\"><body>"; 
    html += "<h1>Monitoramento do Sistema</h1>";
    html += "<div class=\"card\">";
    html += "<h2>Velocidade: " + String(frequenciaAtual) + " Hz</h2>";
    html += "<h2>Sentido: " + String(sentidoHorario ? "Horário" : "Anti-horário") + "</h2>";
    html += "</div></body></html>";
    server.send(200, "text/html", html);
  });
  
  server.begin();
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP); 
  digitalWrite(PIN_DIR, sentidoHorario);

  attachInterrupt(digitalPinToInterrupt(PIN_BTN), inverteSentido, FALLING);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Falha ao iniciar o OLED");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // --- CONFIGURAÇÃO DO GERADOR DE PULSOS ---
  #if !MODO_SIMULADOR
    // Implementação Física: Biblioteca Nativa MCPWM
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PIN_STEP); 
    
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;    
    pwm_config.cmpr_a = 0.0;        
    pwm_config.cmpr_b = 0.0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
  #else
    // Implementação Wokwi: Tone nativo (compatível com o simulador visual)
    pinMode(PIN_STEP, OUTPUT);
  #endif
  
  setupWiFi();
}

void loop() {
  int leituraPot = analogRead(PIN_POT);
  frequenciaAtual = map(leituraPot, 0, 4095, 0, 1500);

  // Filtro de Histerese: Só atualiza os registradores se o valor oscilar
  if (abs(frequenciaAtual - frequenciaAnterior) > 10) {
    if (frequenciaAtual < 15) {
      
      #if !MODO_SIMULADOR
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
      #else
        noTone(PIN_STEP);
      #endif
      
    } else {
      
      #if !MODO_SIMULADOR
        mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
        mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, frequenciaAtual);
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 50.0);
      #else
        tone(PIN_STEP, frequenciaAtual);
      #endif
      
    }
    frequenciaAnterior = frequenciaAtual; 
  }

  // Atualização das interfaces a cada 200ms
  if (millis() - ultimaAtualizacaoOled > 200) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("SISTEMA MOTOR");
    display.drawLine(0, 10, 128, 10, WHITE);
    display.setCursor(0, 20);
    display.print("Vel: ");
    display.print(frequenciaAtual);
    display.print(" Hz");
    display.setCursor(0, 40);
    display.print("Dir: ");
    display.print(sentidoHorario ? "Horario" : "Anti-horario");
    display.setCursor(0, 56);
    display.print("WiFi: Conectado");
    display.display();
    
    ultimaAtualizacaoOled = millis();
  }

  server.handleClient();
}
