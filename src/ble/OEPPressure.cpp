//
// Created by Wilson Wong on 5/16/23.
//

#include <lwip/def.h>
#include <BLE2902.h>
#include <Arduino.h>
#include "OEPPressure.h"

class ZeroCallback : public BLECharacteristicCallbacks {
private:
    OEPPressure *_pressureService;
public:
    explicit ZeroCallback(OEPPressure *pressureService) {
        this->_pressureService = pressureService;
    }

    void onNotify(BLECharacteristic *pCharacteristic) override {
        _pressureService->resetUpdateCounter();
    }

    void onWrite(BLECharacteristic *pCharacteristic) override {
        _pressureService->setZeroPressure();
    }
};

BLECharacteristic PressureCharacteristic(
        BLE_PRESSURE_CHARACTERISTIC,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor PressureDescriptor(BLE_PRESSURE_DESCRIPTOR);

BLECharacteristic ZeroCharacteristic(
        BLE_PRESSURE_ZERO_CHARACTERISTIC,
        BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor ZeroDescriptor(BLE_PRESSURE_ZERO_DESCRIPTOR);

void OEPPressure::updatePressure(int16_t newPressure) {
    this->_lastPressureReading = newPressure;
    uint16_t pressureVal = this->getReportablePresureValue();
    Serial.printf("pressureVal: %x\r\n", pressureVal);
    PressureCharacteristic.setValue(pressureVal);
    PressureCharacteristic.notify();
    this->_updatesSent = (_updatesSent + 1) % 16;

    //if (this->_updatesSent == 15) {
    //    Serial.println("Sending out a bogus temperature");
    //    PressureCharacteristic.setValue(_temperature, 2);
    //    PressureCharacteristic.notify();
    //    this->_updatesSent = 0;
    //}
}

void OEPPressure::resetUpdateCounter() {
    this->_updatesSent = 0;
}

uint16_t OEPPressure::getReportablePresureValue() const {
    auto offsetPressure = int16_t(this->_lastPressureReading - this->_pressureOffset);
    Serial.printf("got reportable pressure %d\r\n", offsetPressure);
    return (uint16_t) (offsetPressure<<8 & 0xff00) | (offsetPressure >> 8 & 0xff);
}

void OEPPressure::setZeroPressure() {
    this->_pressureOffset = this->_lastPressureReading;
}

void OEPPressure::registerWithServer(BLEServer *pServer) {
    auto pressureService = pServer->createService(BLE_PRESSURE_SERVICE);

    pressureService->addCharacteristic(&PressureCharacteristic);
    PressureDescriptor.setValue("notify: pressure followed by temperature at every 16th notification");
    PressureCharacteristic.addDescriptor(&PressureDescriptor);
    PressureCharacteristic.addDescriptor(new BLE2902());

    ZeroDescriptor.setValue("write: any value");
    ZeroCharacteristic.addDescriptor(&ZeroDescriptor);
    ZeroCharacteristic.setCallbacks(new ZeroCallback(this));
    pressureService->addCharacteristic(&ZeroCharacteristic);

    pressureService->start();
    pServer->getAdvertising()->addServiceUUID(BLE_PRESSURE_SERVICE);
}