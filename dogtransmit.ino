#include <WiFi.h>
#include <esp_now.h>

#define SWITCH_PIN 4   // GPIO4 switch pin

// Receiver MAC Address
uint8_t receiverMAC[] = {0x94, 0xA9, 0x90, 0x7C, 0xA8, 0x50};

typedef struct struct_message {
  int senderID;
  bool switchState;   // true = ON, false = OFF
} struct_message;

struct_message dataToSend;
esp_now_peer_info_t peerInfo;

// Send callback
void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

bool lastState = HIGH;

void setup() {

  Serial.begin(115200);

  // Configure switch pin with internal pullup
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);

  Serial.println("ESP-NOW Transmitter");
  Serial.print("My MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer Add Failed");
    return;
  }
}

void loop() {

  bool currentState = digitalRead(SWITCH_PIN);

  // Send only when state changes
  if (currentState != lastState) {

    dataToSend.senderID = 1;

    if (currentState == LOW) {
      // Switch ON (pressed)
      dataToSend.switchState = true;
      Serial.println("Switch ON → Sending ON");
    }
    else {
      // Switch OFF (released)
      dataToSend.switchState = false;
      Serial.println("Switch OFF → Sending OFF");
    }

    esp_err_t result = esp_now_send(receiverMAC,
                                    (uint8_t *)&dataToSend,
                                    sizeof(dataToSend));

    if (result == ESP_OK)
      Serial.println("Sending...");
    else
      Serial.println("Send Error");

    lastState = currentState;
  }

  delay(50); // small debounce delay
}