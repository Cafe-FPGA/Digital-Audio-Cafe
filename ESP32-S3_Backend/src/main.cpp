#include <Arduino.h>
#include "CommsManager.h"
#include "AudioPlayer.h"

// Global Variables
float last_volume = -1.0f;
float fetched_volume = 0.0f;
float fetched_smoothing = 0.0f;

// Global Objects
HardwareSerial ManagerUART(1); // Claims UART 1 (UART 0 is for USB Serial monitor)
CommsManager frontend;
AudioPlayer audio;
SemaphoreHandle_t audioMutex;

void frontendTask(void *pvParameters) {
    while(1) {
        vTaskDelay(10 / portTICK_PERIOD_MS); // delay for 10 ms (polling @100hz)

        frontend.update();

        if (xSemaphoreTake(audioMutex, portMAX_DELAY) == pdTRUE) { // "Take" the key; if not available, wait
            fetched_volume = frontend.getVolume(); // Current "system volume"
            fetched_smoothing = frontend.getSmoothing(); // Current smoothing value
            xSemaphoreGive(audioMutex); // "Give" the key back
        }

        // Debugging CommsManager
        if (fetched_volume != last_volume) {
            Serial.print("esp_volume:");
            Serial.println(fetched_volume);
            last_volume = fetched_volume;
        }
    }
}

void audioTask(void *pvParameters) {
    while(1) {

        if (xSemaphoreTake(audioMutex, portMAX_DELAY) == pdTRUE) { // "Take" the key; if not available, wait
            audio.setVolume(fetched_volume);
            audio.setSmoothingFactor(fetched_smoothing);
            xSemaphoreGive(audioMutex); // "Give" the key back
        }

        audio.playSineWave();

    }
}

void setup() {

    // Hardware inits
    Serial.begin(115200);

    // DEBUGGING 3/29
    delay(3000);
    Serial.println("\n\n--- ESP32-S3 BOOTING ---");

    frontend.init();
    audio.init();
    audio.initSDCard();

    // FreeRTOS tasks
    audioMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(      
        frontendTask,      // 1. Pointer to the task function
        "FrontendTask",    // 2. Name of task (for debugging, e.g., "AudioTask")
        4096,              // 3. Stack size in words (Use 4096 or 8192 for now)
        NULL,              // 4. Parameter to pass to function (Usually NULL)
        1,                 // 5. Task priority (0 is lowest, 1 is normal, configMAX_PRIORITIES-1 is highest)
        NULL,              // 6. Task handle (NULL if you don't need to kill it later)
        0                  // 7. Core ID (0 or 1)
    );

    xTaskCreatePinnedToCore(      
        audioTask,         // 1. Pointer to the task function
        "AudioTask",       // 2. Name of task (for debugging, e.g., "AudioTask")
        4096,              // 3. Stack size in words (Use 4096 or 8192 for now)
        NULL,              // 4. Parameter to pass to function (Usually NULL)
        2,                 // 5. Task priority (0 is lowest, 1 is normal, configMAX_PRIORITIES-1 is highest)
        NULL,              // 6. Task handle (NULL if you don't need to kill it later)
        1                  // 7. Core ID (0 or 1)
    );

}

void loop() {

}