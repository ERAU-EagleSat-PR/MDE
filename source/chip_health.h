/*
 * 
 */
#ifndef SOURCE_CHIP_HEALTH_H_
#define SOURCE_CHIP_HEALTH_H_

#include "source/chips.h"

//
// Data structure for general chip health
//
typedef struct {
    uint8_t HealthCount; // Failures of health check
    uint8_t WatchdogCount; // Times watchdog has been triggered
} CHIPHEALTH;

//
// Function Prototypes
//
uint8_t CheckChipHealth(uint8_t chip_number);

//
// Global chip health array
//
extern CHIPHEALTH chip_health_array[MAX_CHIP_NUMBER];

#define CHIP_MAX_WATCHDOG 2
#define CHIP_MAX_HEALTH 3


#endif // Chip_health.h
