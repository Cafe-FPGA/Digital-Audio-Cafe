#include <Arduino.h>
#include <math.h>
#include "AudioPlayer.h"

void AudioPlayer::init() { // CRANK UP QUALITY UPON COMPLETION OF SD CARD READING LOGIC
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // ESP32 is the boss (Master), Transmitting data (TX)
        .sample_rate = 44100,                                // Standard CD audio sample rate
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,        // 16-bit audio
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,        // Stereo Left/Right
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,   // Standard I2S protocol
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,            // Standard hardware interrupt priority
        .dma_buf_count = 8,                                  // Number of DMA memory buffers
        .dma_buf_len = 64,                                   // Size of each DMA buffer
        .use_apll = true,                                    
        .tx_desc_auto_clear = true                           // Auto-clear data to prevent noise on underflow
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = BCK_PIN,
        .ws_io_num = LCK_PIN,
        .data_out_num = DIN_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    i2s_driver_install(i2s_port, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_port, &pin_config);
}

void AudioPlayer::setVolume(float volume_from_main) { // Volume setter and perceptual volume scaler
    target_volume = (volume_from_main * volume_from_main * volume_from_main);
}

void AudioPlayer::setSmoothingFactor(float smoothing_factor_from_main) { // Smoothing setter
    smoothing_factor = smoothing_factor_from_main;
}

void AudioPlayer::playSineWave() {
    static float angle = 0.0; // Encapsulation
    float angle_step = ((2.0f * 440.0f) * PI) / 44100.0f;
    int16_t sample_data[128]; // 64 stereo frames

    for (int i = 0; i< 64; i++) {
        current_volume = current_volume + (target_volume - current_volume) *smoothing_factor;
        int16_t sample_val = (sin(angle) * (current_volume * 500));
        angle += angle_step;
        if (angle > 2.0 * PI) {
            angle -= (2.0 * PI);
        }
        sample_data[i * 2] = sample_val;
        sample_data[(i * 2) + 1] = sample_val;
    }

    size_t bytes_written;
    i2s_write(i2s_port, &sample_data, sizeof(sample_data), &bytes_written, portMAX_DELAY);
}