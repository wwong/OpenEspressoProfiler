//
// Created by Wilson Wong on 5/16/23.
//

#include <BLE2902.h>
#include <Arduino.h>
#include "OEPLog.h"

BLECharacteristic LogCharacteristic(BLE_LOG_CHARACTERISTIC, BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor LogDescriptor(BLE_LOG_DESCRIPTOR);

void OEPLog::registerWithServer(BLEServer *pServer) {
    auto logService = pServer->createService(BLE_LOG_SERVICE);
    LogDescriptor.setValue("null terminated string");
    logService->addCharacteristic(&LogCharacteristic);
    LogCharacteristic.addDescriptor(&LogDescriptor);
    LogCharacteristic.addDescriptor(new BLE2902());
    pServer->getAdvertising()->addServiceUUID(BLE_LOG_SERVICE);
    logService->start();
}

void OEPLog::log(const std::string &str) {
    Serial.println(str.c_str());
    LogCharacteristic.setValue(str);
    LogCharacteristic.notify();
}

OEPLog::OEPLog() = default;
