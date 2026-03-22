#include <Arduino.h>

#define TX_PIN 4
#define RX_PIN 5

// Global Objects
HardwareSerial ManagerUART(1); // Claims UART 1 (UART 0 is for USB Serial monitor)

void setup() {

    Serial.begin(115200);
    ManagerUART.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

}

void loop() {
    if (ManagerUART.available()) {
        String uart_in = ManagerUART.readStringUntil('\n');
        Serial.println(uart_in);
    }
}