#ifndef OPENESPRESSOPROFILER_CONFIG_H
#define OPENESPRESSOPROFILER_CONFIG_H

#include <Arduino.h>

#ifndef BUILD_ENV_NAME
    #define generic_espwroom32 101
#else
    #define generic_espwroom32 101
    #define xiao_esp32s3 102
#endif

#if !defined(BUILD_ENV_NAME)
#warning "BUILD_ENV_NAME not set. Going to continue with generic configs."
#define LED_PIN 2
#define INVERT_LED false
#define PRESSURE_PIN 32

#elif BUILD_ENV_NAME==generic_espwroom32
#pragma message "Using generic_espwroom32"
#define LED_PIN 2
#define INVERT_LED false
#define PRESSURE_PIN 32

#elif BUILD_ENV_NAME==xiao_esp32s3
#pragma message "Using xiao_esp32s3"
#define LED_PIN LED_BUILTIN
#define INVERT_LED true
#define PRESSURE_PIN A1

#else
#error "BUILD_ENV_NAME NOT RECOGNIZED"
#endif


#define R1_OHMS 10000
#define R2_OHMS 15000

#define TRANSDUCER_MAX_PSI 200
#define TRANSDUCER_MIN_V 0.5
#define TRANSDUCER_MAX_V 4.5

#endif //OPENESPRESSOPROFILER_CONFIG_H
