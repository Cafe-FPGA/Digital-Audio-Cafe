#include <Arduino.h>
#include "CommsManager.h"
#include "AudioPlayer.h"

// Global Variables
float last_volume = -1.0f;

// Global Objects
HardwareSerial ManagerUART(1); // Claims UART 1 (UART 0 is for USB Serial monitor)
CommsManager frontend;
AudioPlayer audio;

void setup() {

    Serial.begin(115200);
    frontend.init();
    audio.init();

}

void loop() {

    frontend.update();

    float fetched_volume = frontend.getVolume(); // Current "system volume"
    float fetched_smoothing = frontend.getSmoothing(); // Current smoothing value
    
    audio.setVolume(fetched_volume);
    audio.setSmoothingFactor(fetched_smoothing);

    // Debugging CommsManager
    if (fetched_volume != last_volume) {
        Serial.print("esp_volume:");
        Serial.println(fetched_volume);
        last_volume = fetched_volume;
    }

    audio.playSineWave();

}