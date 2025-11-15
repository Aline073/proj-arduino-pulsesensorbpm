#include <WiFi.h>
#include <PubSubClient.h>
#include <PulseSensorPlayground.h>

// --- Pinos ---
const int PulseWire = 32;
const int LED_PIN = 2;
const int BUZZER_PIN = 25;

// --- WiFi / MQTT ---
const char* SSID = "Nome da rede";
const char* PASSWORD = "Senha da rede";
const char* BROKER_MQTT = "IP da máquina";
int BROKER_PORT = 1883;
#define TOPICO_SUBSCRIBE "MQTTEnvia"
#define TOPICO_PUBLISH   "MQTTRecebe"
#define ID_MQTT  "HomeAut"

// --- PulseSensor ---
PulseSensorPlayground pulseSensor;
unsigned long calibStart = 0;
const unsigned long CALIB_DURATION = 60000UL;  // 60 segundos
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
  Serial.println("Calibrando por 60s...");

  initWiFi();
  initMQTT();
}

// --- Loop principal ---
void loop() {
  unsigned long now = millis();

  if (!calibrated) {
    int raw = analogRead(PulseWire);
    rawMin = min(rawMin, raw);
    rawMax = max(rawMax, raw);

    if (now - calibStart >= CALIB_DURATION) {
      int threshold = max((rawMin + rawMax) / 2, 200);
      pulseSensor.setThreshold(threshold);
      calibrated = true;
      Serial.println("Calibracao concluida. Threshold: " + String(threshold));
    }
    delay(20);
    return;
  }

  VerificaConexoesWiFIEMQTT();
  MQTT.loop();

  if (pulseSensor.sawStartOfBeat()) {
    int myBPM = pulseSensor.getBeatsPerMinute();
    Serial.println("BPM: " + String(myBPM));

    char buf[8];
    sprintf(buf, "%d", myBPM);
    MQTT.publish("BPM/Leitura", buf);

    // --- Controle do buzzer conforme BPM ---
    if (myBPM > 0 && (myBPM < 60 || myBPM > 140)) {
      noTone(BUZZER_PIN);
      tone(BUZZER_PIN, TONE_CONTINUOUS_HZ);
      intermittentMode = false;
      intermittentOn = true;
    } else if (myBPM >= 60 && myBPM <= 140) {
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
