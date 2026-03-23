#include <Arduino.h>
#include "CommsManager.h"

extern HardwareSerial ManagerUART;

CommsManager::CommsManager() {
    rx_index = 0;
    current_volume = 0;
}

void CommsManager::update() {

    if (ManagerUART.available() > 0) {
        char incoming_byte = ManagerUART.read();
        if (incoming_byte == '\n') {
            rx_buffer[rx_index] = '\0';
            int volume_found = sscanf(rx_buffer, "VOL:%d", &current_volume);
            rx_index = 0;
        } 
        else if (rx_index < 19) { // 19 leaves room for the '\0' at the end
            rx_buffer[rx_index] = incoming_byte;
            rx_index++;
        }
    }

}

int CommsManager::getVolume() {
    return current_volume;
}