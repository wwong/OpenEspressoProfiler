//
// Created by Wilson Wong on 5/16/23.
//

#ifndef UNTITLED_BLEBATTERY_H
#define UNTITLED_BLEBATTERY_H

#include <BLEUtils.h>

#define BLE_BATTERY_SERVICE BLEUUID((uint16_t) ESP_GATT_UUID_BATTERY_SERVICE_SVC)
#define BLE_BATTERY_CHARACTERISTIC BLEUUID((uint16_t) ESP_GATT_UUID_BATTERY_LEVEL)
#define BLE_BATTERY_DESCRIPTOR BLEUUID((uint16_t) ESP_GATT_UUID_CHAR_DESCRIPTION)

class BLEBattery {
private:
    uint8_t _battery_level = 100;

public:
    explicit BLEBattery(uint8_t initialLevel);
    void batteryLevel(uint8_t newLevel);
    uint8_t batterylevel() const;
    void setupBatteryService(BLEServer *pServer);
};
#endif //UNTITLED_BLEBATTERY_H
