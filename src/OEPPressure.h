//
// Created by Wilson Wong on 5/16/23.
//

#ifndef UNTITLED_OEPPRESSURE_H
#define UNTITLED_OEPPRESSURE_H
#include "BLEUtils.h"

#define BLE_PRESSURE_SERVICE BLEUUID("873ae82a-4c5a-4342-b539-9d900bf7ebd0")
#define BLE_PRESSURE_CHARACTERISTIC "873ae82b-4c5a-4342-b539-9d900bf7ebd0"
#define BLE_PRESSURE_DESCRIPTOR BLEUUID((uint16_t) ESP_GATT_UUID_CHAR_DESCRIPTION)
#define BLE_PRESSURE_ZERO_CHARACTERISTIC "873ae82c-4c5a-4342-b539-9d900bf7ebd0"
#define BLE_PRESSURE_ZERO_DESCRIPTOR BLEUUID((uint16_t) ESP_GATT_UUID_CHAR_DESCRIPTION)

class OEPPressure {
private:
    int16_t _pressureOffset = 0;
    int16_t _lastPressureReading = 0;
    uint8_t _temperature[2] = {0, 0};
    uint8_t _updatesSent = 0;
public:
    OEPPressure() = default;

    void updatePressure(int16_t newPressure);
    void setZeroPressure();
    void registerWithServer(BLEServer *pServer);

    uint16_t getReportablePresureValue() const;

    void resetUpdateCounter();
};


#endif //UNTITLED_OEPPRESSURE_H
