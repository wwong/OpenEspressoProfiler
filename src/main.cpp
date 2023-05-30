#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <esp_bt_main.h>
#include "esp_bt_device.h"
#include "string"

#include "BLEBattery.h"
#include "OEPLog.h"
#include "OEPPressure.h"

#define LED_PIN 2
#define PRESSURE_PIN 32

#define PRESSURE_M 0.004495915872
#define PRESSURE_B 0.4350627433

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

int16_t readPressureMillibars() {
    uint16_t adcValue = analogRead(PRESSURE_PIN);
    // Transducer parameters
    // Taken from a linear regression of measured pressure in Bars (y) vs ADC readings (x)
    return floor(1000.0 * ((float(adcValue) * PRESSURE_M) - PRESSURE_B));
}

unsigned long lastPoll = 0;

void loop() {
    unsigned long now = millis();
    if (now - lastPoll > 250) {
        Serial.println("ping");
        lastPoll = now;

        digitalWrite(LED_PIN, deviceConnected);

        if (PRESSURE != nullptr) {
            int16_t pressure = readPressureMillibars();
            Serial.print("Reporting new pressure reading: ");
            Serial.println(pressure);
            PRESSURE->updatePressure(pressure);
        }
    }
}