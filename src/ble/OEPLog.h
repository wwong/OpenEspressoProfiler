//
// Created by Wilson Wong on 5/16/23.
//

#ifndef UNTITLED_OEPLOG_H
#define UNTITLED_OEPLOG_H
#include <BLEUtils.h>

#define BLE_LOG_SERVICE BLEUUID("873ae828-4c5a-4342-b539-9d900bf7ebd0")
#define BLE_LOG_CHARACTERISTIC BLEUUID("873ae829-4c5a-4342-b539-9d900bf7ebd0")
#define BLE_LOG_DESCRIPTOR BLEUUID((uint16_t) ESP_GATT_UUID_CHAR_DESCRIPTION)

class OEPLog {
public:
    OEPLog();
    void registerWithServer(BLEServer *pServer);
    void log(const std::string &str);
};


#endif //UNTITLED_OEPLOG_H
