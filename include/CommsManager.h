#ifndef COMMS_MANAGER_H
#define COMMS_MANAGER_H

class CommsManager {

private:
    char rx_buffer[20];
    int rx_index = 0;
    int current_volume;

public:
    CommsManager();

    void update();
    int getVolume();
};

#endif