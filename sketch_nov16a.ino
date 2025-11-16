#include <WiFi.h>
#include <PubSubClient.h>
#include <PulseSensorPlayground.h>

// --- Pinos ---
const int PulseWire = 32;
const int LED_PIN = 2;
const int BUZZER_PIN = 25;

// --- WiFi / MQTT ---
const char* SSID = "Nome rede";
const char* PASSWORD = "Senha Rede";
const char* BROKER_MQTT = "IP";
int BROKER_PORT = 1883;
#define TOPICO_SUBSCRIBE "MQTTEnvia"
#define TOPICO_PUBLISH   "MQTTRecebe"
#define ID_MQTT  "HomeAut"

// --- PulseSensor ---
PulseSensorPlayground pulseSensor;
unsigned long calibStart = 0;
const unsigned long CALIB_DURATION = 30000UL;
int rawMin = 4095, rawMax = 0;
bool calibrated = false;

// --- Estado ---
char EstadoSaida = '0';

// --- Buzzer ---
bool intermittentMode = false;
bool intermittentOn = false;
unsigned long intermittentTimer = 0;
#define TONE_CONTINUOUS_HZ 1000
#define TONE_INTERMITTENT_HZ 2000
#define BEEP_ON 100
#define BEEP_OFF 300

// --- MQTT ---
WiFiClient espClient;
PubSubClient MQTT(espClient);

// --- Controle de medições ---
const int maxMedicoes = 4;
int sensorCount = 0;
int buzzerCount = 0;
bool testesConcluidos = false;

// --- Setup ---
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetPinAttenuation(PulseWire, ADC_11db);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(1500);

  if (pulseSensor.begin()) Serial.println("PulseSensor iniciado");
  else Serial.println("Erro ao iniciar PulseSensor");

  calibStart = millis();
  Serial.println("Calibrando por 30s...");

  initWiFi();
  initMQTT();
}

// --- Loop principal ---
void loop() {
  unsigned long now = millis();

  // --- Fase 1: Calibração ---
  if (!calibrated) {
    int raw = analogRead(PulseWire);
    rawMin = min(rawMin, raw);
    rawMax = max(rawMax, raw);

    if (now - calibStart >= CALIB_DURATION) {
      int threshold = max((rawMin + rawMax) / 2, 200);
      pulseSensor.setThreshold(threshold);
      calibrated = true;
      Serial.println("Calibracao concluida. Threshold: " + String(threshold));
      Serial.println("Iniciando testes de tempo...");
    }
    delay(20);
    return;
  }

  // --- Fase 2: Testes de tempo ---
  if (!testesConcluidos) {
    // Sensor
    if (sensorCount < maxMedicoes) {
      unsigned long tStart = micros();
      int raw = analogRead(PulseWire);
      unsigned long tEnd = micros();
      double tempo = (double)(tEnd - tStart) / 1000.0; // ms
      Serial.printf("Sensor - Medida %d Tempo: %.6f ms\n", sensorCount + 1, tempo);
      sensorCount++;
      delay(500);
      return;
    }

    // Buzzer
    if (buzzerCount < maxMedicoes) {
      unsigned long tStart = micros();
      tone(BUZZER_PIN, 1000, 100);
      unsigned long tEnd = micros();
      double tempo = (double)(tEnd - tStart) / 1000.0; // ms
      Serial.printf("Buzzer - Medida %d Tempo: %.6f ms\n", buzzerCount + 1, tempo);
      buzzerCount++;
      delay(500);
      return;
    }

    // Finaliza testes
    if (sensorCount >= maxMedicoes && buzzerCount >= maxMedicoes) {
      Serial.println("Testes de tempo concluídos! Iniciando leituras de BPM...");
      testesConcluidos = true;
    }
    return;
  }

  // --- Fase 3: funcionamento normal com BPM + MQTT ---
  VerificaConexoesWiFIEMQTT();
  MQTT.loop();

  if (pulseSensor.sawStartOfBeat()) {
    int myBPM = pulseSensor.getBeatsPerMinute();
    Serial.println("BPM: " + String(myBPM));

    char buf[8];
    sprintf(buf, "%d", myBPM);
    MQTT.publish("BPM/Leitura", buf);

    // --- Controle do buzzer conforme BPM ---
    if (myBPM > 0 && myBPM < 60) {
      noTone(BUZZER_PIN);
      tone(BUZZER_PIN, TONE_CONTINUOUS_HZ);
      intermittentMode = false;
      intermittentOn = true;
    } else if (myBPM >= 60 && myBPM <= 120) {
      noTone(BUZZER_PIN);
      intermittentMode = true;
      intermittentOn = false;
      intermittentTimer = now;
    } else {
      noTone(BUZZER_PIN);
      intermittentMode = false;
      intermittentOn = false;
    }
  }

  // --- Controle do som intermitente ---
  if (intermittentMode) {
    if (intermittentOn) {
      if (now - intermittentTimer >= BEEP_ON) {
        noTone(BUZZER_PIN);
        intermittentOn = false;
        intermittentTimer = now;
      }
    } else {
      if (now - intermittentTimer >= BEEP_OFF) {
        tone(BUZZER_PIN, TONE_INTERMITTENT_HZ);
        intermittentOn = true;
        intermittentTimer = now;
      }
    }
  }

  delay(20);
}

// --- WiFi ---
void initWiFi() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado. IP: " + WiFi.localIP().toString());
}

// --- MQTT ---
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Tentando conectar ao MQTT...");
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    } else {
      Serial.println("Falha. Tentando em 2s...");
      delay(2000);
    }
  }
}

void VerificaConexoesWiFIEMQTT() {
  if (WiFi.status() != WL_CONNECTED) initWiFi();
  if (!MQTT.connected()) reconnectMQTT();
}

// --- Callback MQTT ---
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.println("MQTT recebido: " + msg);

  if (msg == "L") {
    setLedState(true);
  } else if (msg == "D") {
    setLedState(false);
  } else if (msg == "B") {
    tone(BUZZER_PIN, 1000, 200);
  }

  EnviaEstadoOutputMQTT();
}

// --- Publica estado do LED ---
void EnviaEstadoOutputMQTT() {
  if (EstadoSaida == '0') MQTT.publish(TOPICO_PUBLISH, "D");
  else MQTT.publish(TOPICO_PUBLISH, "L");
  Serial.println("Estado LED enviado ao broker");
}

// --- Controle do LED ---
void setLedState(bool on) {
  if (on) digitalWrite(LED_PIN, LOW);
  else digitalWrite(LED_PIN, HIGH);
  EstadoSaida = on ? '1' : '0';
}