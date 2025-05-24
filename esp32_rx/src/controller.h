// #ifndef controller_h
// #define controller_h

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <string.h>

#pragma pack(1)
struct CONTROLLER_READOUT
{
  struct
  {
    uint8_t A0, A3;
    uint8_t PB10, A4, B13;
    uint8_t A5;
  } LS; // left shoulder
  struct
  {
    uint8_t B12, A6, B2, A7;
  } LD; // left dpad

  struct
  {
    uint8_t C13, A1;
    uint8_t A12, B9, PB11;
    uint8_t B8;
  } RS; // right shoulder
  struct
  {
    uint8_t B3, B5, A15, B4;
  } RD; // right dpad

  struct
  {
    uint8_t A2, C15, C14;
  } ALT; // alternate button

  struct
  {
    uint8_t BTN;
    uint16_t VALUE[2];
  } JOY; // joystick

  struct
  {
    uint8_t BTN;
    uint8_t VALUE;
  } ENC; // encoder
};
#pragma pack()

CONTROLLER_READOUT controller;

bool connected = 0;

void readMacAddress()
{
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK)
  {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  }
  else
  {
    Serial.println("Failed to read MAC address");
  }
}

void logData()
{
  // Log the controller data to serial
  Serial.println("Controller Data:");

  // Left shoulder
  Serial.println("LS (Left Shoulder):");
  Serial.printf("  A0: %u, A3: %u, PB10: %u, A4: %u, B13: %u, A5: %u\n",
                controller.LS.A0, controller.LS.A3, controller.LS.PB10,
                controller.LS.A4, controller.LS.B13, controller.LS.A5);

  // Left dpad
  Serial.println("LD (Left DPad):");
  Serial.printf("  B12: %u, A6: %u, B2: %u, A7: %u\n",
                controller.LD.B12, controller.LD.A6, controller.LD.B2, controller.LD.A7);

  // Right shoulder
  Serial.println("RS (Right Shoulder):");
  Serial.printf("  C13: %u, A1: %u, A12: %u, B9: %u, PB11: %u, B8: %u\n",
                controller.RS.C13, controller.RS.A1, controller.RS.A12,
                controller.RS.B9, controller.RS.PB11, controller.RS.B8);

  // Right dpad
  Serial.println("RD (Right DPad):");
  Serial.printf("  B3: %u, B5: %u, A15: %u, B4: %u\n",
                controller.RD.B3, controller.RD.B5, controller.RD.A15, controller.RD.B4);

  // Alternate button
  Serial.println("ALT (Alternate Button):");
  Serial.printf("  A2: %u, C15: %u, C14: %u\n",
                controller.ALT.A2, controller.ALT.C15, controller.ALT.C14);

  // Joystick
  Serial.println("JOY (Joystick):");
  Serial.printf("  BTN: %u, VALUE[0]: %u, VALUE[1]: %u\n",
                controller.JOY.BTN, controller.JOY.VALUE[0], controller.JOY.VALUE[1]);

  // Encoder
  Serial.println("ENC (Encoder):");
  Serial.printf("  BTN: %u, VALUE: %u\n",
                controller.ENC.BTN, controller.ENC.VALUE);

  Serial.println();
}

void resetController()
{
  // Left shoulder
  controller.LS.A0 = 0;
  controller.LS.A3 = 0;
  controller.LS.PB10 = 0;
  controller.LS.A4 = 0;
  controller.LS.B13 = 0;
  controller.LS.A5 = 0;

  // Left dpad
  controller.LD.B12 = 0;
  controller.LD.A6 = 0;
  controller.LD.B2 = 0;
  controller.LD.A7 = 0;

  // Right shoulder
  controller.RS.C13 = 0;
  controller.RS.A1 = 0;
  controller.RS.A12 = 0;
  controller.RS.B9 = 0;
  controller.RS.PB11 = 0;
  controller.RS.B8 = 0;

  // Right dpad
  controller.RD.B3 = 0;
  controller.RD.B5 = 0;
  controller.RD.A15 = 0;
  controller.RD.B4 = 0;

  // Alternate button
  controller.ALT.A2 = 0;
  controller.ALT.C15 = 0;
  controller.ALT.C14 = 0;

  // Joystick
  controller.JOY.BTN = 0;

  // Encoder
  controller.ENC.BTN = 1;
}

long long int prev_data_recv = 0;
const int controller_timeout = 350;

void checkControllerTimeout() {
  if (millis() - prev_data_recv >= controller_timeout) {
    resetController();
    connected = 0;
  }
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  if (connected == 0) {
    connected = 1;
    Serial.println("Controller connected");
  }
  else {
    prev_data_recv = millis();
  }
  memcpy(&controller, incomingData, sizeof(controller));
  // logData();
}

void initController()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  Serial.print("[DEFAULT] ESP32 Board MAC Address: ");
  readMacAddress();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  resetController();
  logData();
}

// #endif