/*
 * 
 */
#ifndef SOURCE_CHIP_HEALTH_H_
#define SOURCE_CHIP_HEALTH_H_


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
void InitializeChipHealth(void);
void IncrementChipFails(uint8_t chip_number);



//
// Global chip health array
//
extern CHIPHEALTH chip_health_array[32];
extern bool chip_death_array[32];

#define CHIP_WD_MAX 2
#define CHIP_HEALTH_MAX 3


#endif // Chip_health.h
