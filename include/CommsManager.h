#ifndef COMMS_MANAGER_H
#define COMMS_MANAGER_H

class CommsManager {

private:
    static const int TX_PIN = 4;
    static const int RX_PIN = 5;
    char rx_buffer[20];
    int rx_index = 0;
    int current_volume;

public:
    CommsManager();

    void init();
    void update();
    int getVolume();
};

#endif