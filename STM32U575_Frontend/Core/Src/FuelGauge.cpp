#include "FuelGauge.h"

FuelGauge::FuelGauge(I2C_HandleTypeDef* handle) : i2c_handle(handle) {
    // Initialization via member initializer list
}

bool FuelGauge::ping() {
    uint16_t version_data = 0;

    // Entirely removed HAL_I2C_IsDeviceReady. 
    // No more "naked pings".

    // Give the IC a tiny moment to breathe after power-up or bus recovery
    HAL_Delay(5);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        i2c_handle,                    
        (I2C_ADDR_7BIT << 1),    
        REG_IC_VERSION,          
        I2C_MEMADD_SIZE_8BIT,    
        (uint8_t*)&version_data,      
        2,                             
        100                         
    );

    // Place a breakpoint on the (void) line below.
    uint32_t i2c_error = i2c_handle->ErrorCode;
    (void)i2c_error;

    if (status == HAL_OK && version_data > 0) {
        return true;
    } else {
        return false;
    }
}

bool FuelGauge::write_register(uint8_t reg, uint16_t value) {
    // 1. The IC requires the CRC to be calculated over 4 specific bytes:
    // [I2C Address (Write), Register Address, Data Low, Data High]
    uint8_t crc_buffer[4];
    crc_buffer[0] = (I2C_ADDR_7BIT << 1); 
    crc_buffer[1] = reg;
    crc_buffer[2] = value & 0xFF;         // Extract the Lower Byte
    crc_buffer[3] = (value >> 8) & 0xFF;  // Extract the Upper Byte

    // 2. Calculate the cryptographic wax seal
    uint8_t checksum = crc8(crc_buffer, 4);

    // 3. Pack the payload to send over the physical wire:[Data Low, Data High, Checksum]
    uint8_t tx_buffer[3];
    tx_buffer[0] = crc_buffer[2];
    tx_buffer[1] = crc_buffer[3];
    tx_buffer[2] = checksum;

    // 4. Sending exactly 3 bytes
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
        i2c_handle,                    
        (I2C_ADDR_7BIT << 1),    
        reg,          
        I2C_MEMADD_SIZE_8BIT,    
        tx_buffer,      
        3,                             
        100 
    );

    if (status == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

bool FuelGauge::init_battery() {
    bool success = true;

    // 1. Wake the IC into Operational Mode
    if (write_register(REG_POWER_MODE, 0x0001) == false) {
        success = false;
    }

    // 2. Set the APA to 0x002D (Tuning for 2000mAh)
    if (write_register(REG_APA, 0x002D) == false) {
        success = false;
    }

    // 3. Set the Battery Profile to 0x0001 (Standard 3.7V LiPo)
    if (write_register(REG_BATTERY_PROFILE, 0x0001) == false) {
        success = false;
    }

    // 4. Send the Initialization Magic Word (0xAA55)
    if (write_register(REG_INIT_RSOC, 0xAA55) == false) {
        success = false;
    }

    // Wait for the gauge to finish its initial calculation
    HAL_Delay(10);
    return success;
}

uint16_t FuelGauge::read_percentage() {
    uint16_t percentage = 0;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        i2c_handle,                    
        (I2C_ADDR_7BIT << 1),    
        REG_RSOC,          
        I2C_MEMADD_SIZE_8BIT,    
        (uint8_t*)&percentage,      
        2,                             
        100                         
    );

    if (status == HAL_OK) {
        return percentage;
    } else {
        return 0; // If the physical read fails, return 0 for safety.
    }
}

uint16_t FuelGauge::read_voltage() {
    uint16_t voltage = 0;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        i2c_handle,                    
        (I2C_ADDR_7BIT << 1),    
        REG_CELL_VOLTAGE,          
        I2C_MEMADD_SIZE_8BIT,    
        (uint8_t*)&voltage,      
        2,                             
        100                         
    );

    if (status == HAL_OK) {
        return voltage;
    } else {
        return 0; 
    }
}

uint8_t FuelGauge::crc8(uint8_t* data, uint8_t len) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07; // 0x07 is the CRC-8-ATM polynomial
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}