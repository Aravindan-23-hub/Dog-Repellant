#include <WiFi.h>
#include <esp_now.h>

// Pin definitions
const int switchPin = 4;
const int outputPin = 10;
const int buzzerPin = 7;
const int uspin = 5;

// Structure
typedef struct struct_message {
  int senderID;
  int counter;
} struct_message;

struct_message receivedData;

volatile int receivedCommand = 0;

// ESP-NOW receive callback
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {

  memcpy(&receivedData, data, sizeof(receivedData));

  Serial.println("Data Received:");
  Serial.print("Sender ID: ");
  Serial.println(receivedData.senderID);

  Serial.print("Counter: ");
  Serial.println(receivedData.counter);
  Serial.println("----------------");

  receivedCommand = receivedData.counter;
}

void setup() {

  Serial.begin(115200);

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(outputPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(uspin, OUTPUT);

  digitalWrite(outputPin, LOW);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(uspin, LOW);

  WiFi.mode(WIFI_STA);

  Serial.println("ESP-NOW Receiver");
  Serial.print("My MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  int switchState = digitalRead(switchPin);

  // Activate ONLY if:
  // receivedCommand == 1 AND switch is HIGH
  if (switchState == HIGH){
    digitalWrite(outputPin, HIGH);
    delay(100);
  }
  if (receivedCommand == 1 && switchState == HIGH) {

    Serial.println("Command = 1 and Switch HIGH → OUTPUT ON");
    digitalWrite(uspin, HIGH);
    digitalWrite(buzzerPin, HIGH);

  }
  else {

    digitalWrite(outputPin, LOW);
    digitalWrite(uspin, LOW);
    digitalWrite(buzzerPin, LOW);

  }
  //digitalWrite(outputPin, LOW);

  delay(50);
}