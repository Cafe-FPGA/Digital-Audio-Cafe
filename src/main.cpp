#include <Arduino.h>
#include "CommsManager.h"

#define TX_PIN 4
#define RX_PIN 5

// Global Variables
int last_volume = -1;

// Global Objects
HardwareSerial ManagerUART(1); // Claims UART 1 (UART 0 is for USB Serial monitor)
CommsManager frontend;

void setup() {

    Serial.begin(115200);
    ManagerUART.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

}

void loop() {

    frontend.update();

    int fetched_volume = frontend.getVolume();

    // Debugging CommsManager
    if (fetched_volume != last_volume) {
        Serial.print("esp_volume:");
        Serial.println(fetched_volume);

        last_volume = fetched_volume;
    }

}