//
// Created by Wilson Wong on 5/16/23.
//

#include <BLE2902.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEDevice.h>

#include "BLEBattery.h"

BLECharacteristic BatteryLevelCharacteristic(
        BLE_BATTERY_CHARACTERISTIC,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BatteryLevelDescriptor(BLE_BATTERY_DESCRIPTOR);

BLEBattery::BLEBattery(uint8_t initialLevel) {
    this->_battery_level = initialLevel;
}

void BLEBattery::batteryLevel(uint8_t newLevel) {
    this->_battery_level = newLevel;
    BatteryLevelCharacteristic.setValue(&_battery_level, 1);
    BatteryLevelCharacteristic.notify();
}

uint8_t BLEBattery::batterylevel() const {
    return _battery_level;
}

void BLEBattery::setupBatteryService(BLEServer *pServer) {
    BLEService *batteryService = pServer->createService(BLE_BATTERY_SERVICE);
    batteryService->addCharacteristic(&BatteryLevelCharacteristic);
    BatteryLevelDescriptor.setValue("1..100%");
    BatteryLevelCharacteristic.addDescriptor(&BatteryLevelDescriptor);
    BatteryLevelCharacteristic.addDescriptor(new BLE2902());
    pServer->getAdvertising()->addServiceUUID(BLE_BATTERY_SERVICE);
    batteryService->start();
}
