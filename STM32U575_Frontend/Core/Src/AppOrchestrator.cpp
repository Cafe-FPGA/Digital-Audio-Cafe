#include "AppOrchestrator.h"

AppOrchestrator::AppOrchestrator(I2C_HandleTypeDef* hi2c_battery) : battery_monitor(hi2c_battery) {

}

void AppOrchestrator::init() {
    // 1. Check if the IC is alive
    bool is_battery_alive = battery_monitor.ping();
    
    // 2. If it is alive, wake it up and configure it
    if (is_battery_alive) {
        bool is_initialized = battery_monitor.init_battery();
        (void)is_initialized; // (Optional) Breakpoint here to verify it returns true
    }
}

void AppOrchestrator::run() {
    uint16_t current_battery = battery_monitor.read_percentage();
    uint16_t current_voltage = battery_monitor.read_voltage(); // Returns millivolts!
    
    HAL_Delay(1000); 

    (void)current_battery; 
    (void)current_voltage; // Breakpoint here!
}