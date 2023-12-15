#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <esp_bt_main.h>
#include "esp_bt_device.h"
#include "string"

#include "ble/BLEBattery.h"
#include "ble/OEPLog.h"
#include "ble/OEPPressure.h"
#include "config.h"


#define PSI_TO_BAR 0.0689476

OEPLog *LOG;
BLEBattery *BATTERY;
OEPPressure *PRESSURE;
bool deviceConnected;

bool initBluetooth() {
    Serial.println("Init Bluetooth");

    if (!btStart()) {
        Serial.println("Failed to initialize controller");
        return false;
    }

    if (esp_bluedroid_init() != ESP_OK) {
        Serial.println("Failed to initialize bluedroid");
        return false;
    }

    if (esp_bluedroid_enable() != ESP_OK) {
        Serial.println("Failed to enable bluedroid");
        return false;
    }

    return true;
}

void initGpio() {
    Serial.println("Init GPIO");
    pinMode(LED_PIN, OUTPUT);
    pinMode(PRESSURE_PIN, INPUT);
    digitalWrite(LED_PIN, LOW);
}

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        pServer->startAdvertising();
    }
};

void setup() {
    Serial.begin(115200);

    initGpio();
    initBluetooth();

    Serial.println("creating log");
    LOG = new OEPLog();
    Serial.println("creating battery");
    BATTERY = new BLEBattery(100);
    Serial.println("creating pressure");
    PRESSURE = new OEPPressure();

    const uint8_t *btMac = esp_bt_dev_get_address();
    char nameSuffix[7];
    if (btMac != nullptr) {
        Serial.printf("%p", btMac);
        sprintf(nameSuffix, "%02x%02x%02x", btMac[3], btMac[4], btMac[5]);
    } else {
        Serial.println("Unknown local MAC");
        nameSuffix[0] = 'O';
        nameSuffix[1] = 'E';
        nameSuffix[2] = 'P';
        nameSuffix[3] = 0;
    }
    std::string deviceName = "PRS-" + std::string(nameSuffix);


    Serial.print("Advertising Bluetooth as ");
    Serial.println(deviceName.c_str());

    BLEDevice::init(deviceName);
    BLEServer *pServer = BLEDevice::createServer();

    pServer->setCallbacks(new MyServerCallbacks());

    Serial.println("Attaching battery service");
    BATTERY->setupBatteryService(pServer);

    Serial.println("Attaching logger service");
    LOG->registerWithServer(pServer);

    Serial.println("Attaching pressure service");
    PRESSURE->registerWithServer(pServer);

    Serial.println("Begin BT advertising");
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    pAdvertising->start();
}

double readAdcVoltage(uint8_t pin) {
    double calibration = 1.000;
    uint32_t adcValue = analogReadMilliVolts(pin) * calibration;
    return double(adcValue);
}

int16_t readPressure() {
    // Voltage divider values
    uint16_t r1 = R1_OHMS, r2 = R2_OHMS;
    // Pressure transducer values
    uint16_t maxPsi = TRANSDUCER_MAX_PSI;
    float minV = TRANSDUCER_MIN_V, maxV = TRANSDUCER_MAX_V;

    // Calculated values
    double measuredMillivolts = readAdcVoltage(PRESSURE_PIN) / 1000.0;
    Serial.printf("Measured Voltage: %f\n", measuredMillivolts);
    float voltageDividerFactor = float(r2) / float(r1 + r2);
    double rescaledVoltage = measuredMillivolts / voltageDividerFactor;
    Serial.printf("Rescaled Voltage: %f\n", rescaledVoltage);
    double psiFactor = (rescaledVoltage - minV) / (maxV - minV);
    Serial.printf("PSI: %f\n", maxPsi * psiFactor);
    return int16_t(1000 * maxPsi * psiFactor * PSI_TO_BAR);
}

unsigned long lastPoll = 0;

void loop() {
    unsigned long now = millis();
    if (now - lastPoll > 100) {
        Serial.println("ping");
        lastPoll = now;

        digitalWrite(LED_PIN, INVERT_LED ^ deviceConnected);

        if (PRESSURE != nullptr) {
            int16_t pressure = readPressure();
            Serial.printf("Reporting new pressure reading: %d\n", pressure);
            PRESSURE->updatePressure(pressure);
        }
    }
}