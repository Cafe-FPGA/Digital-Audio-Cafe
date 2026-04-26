#pragma once
#include <stdint.h>
#include "main.h"

class FuelGauge {
private:
    I2C_HandleTypeDef* i2c_handle;
    static const uint8_t I2C_ADDR_7BIT = 0x0B;
    static const uint8_t REG_IC_VERSION = 0x11;
    static const uint8_t REG_APA = 0x0B;
    static const uint8_t REG_INIT_RSOC = 0x07;
    static const uint8_t REG_BATTERY_PROFILE = 0x12;
    static const uint8_t REG_POWER_MODE = 0x15;
    static const uint8_t REG_RSOC = 0x0D;
    static const uint8_t REG_CELL_VOLTAGE = 0x09;

public:
    FuelGauge(I2C_HandleTypeDef* handle); // Dependency injection
    bool ping(); // Return true if IC responds valid
    bool write_register(uint8_t reg, uint16_t value);
    bool init_battery();
    uint16_t read_percentage();
    uint16_t read_voltage();
    uint8_t crc8(uint8_t* data, uint8_t len);
};