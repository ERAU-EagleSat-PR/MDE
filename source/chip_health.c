/*
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware and Driver files
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
// Chip Drivers
#include "chipDrivers/FLASHfunc.h"
#include "chipDrivers/FRAMfunc.h"
#include "chipDrivers/MRAMfunc.h"
#include "chipDrivers/SRAMfunc.h"

// Local Project files
#include "source/chips.h"
#include "source/chip_health.h"

#include "source/UART0_func.h"

// Update this function to use multiple levels of failure; IE, if a chip fails three times in a row, it should be considered DEAD permanently.
// Alternatively if the watchdog is triggered twice should be considered dead as well
// Will be reset on power cycle
// Can be stored in a struct.
uint8_t
CheckChipHealth(uint8_t chip_number)
{
    // This function will take any given chip number 0-31 and execute the correct health check.
    // It will then update the global chip health array with a success or failure.
    // Optionally returns a value of 0 for a pass and a non-zero value of how many failures otherwise

    uint8_t chip_number_norm = chip_number % 16;
    uint8_t chip_health = 0; // Set to 0 by default, if a problem is detected the value will be incremented.

    if (chip_number_norm < 4)       // Flash
    {
        // Retrieve the Flash ID info and status register.
        FLASHID FLASH_register = FlashStatusRead(chip_number);

        // One mismatch will be a fail condition
        if(FLASH_register.cypID != FLASH_CYP_ID          || FLASH_register.RDSR & 0x01                // Option here to check if the WIP bit of the register is a 1 or not.
                || FLASH_register.prodID1 != FLASH_PROD1 || FLASH_register.prodID2 != FLASH_PROD2)    // I believe it may be stuck 1 if the chip dies, but could also be a 1 even if chip is not dead.
        {                                                                                             // however if it is still 1 on a new cycle, safe to assume chip is stuck or dead
            chip_health = chip_health_array[chip_number].HealthCount + 1; // Increment value in array by 1 if failed
        }
    }
    else if (chip_number_norm < 8)  // FRAM
    {
        // Retrieve the contents of the FRAM ID register
        FRAMID FRAM_register = FRAMStatusRead(chip_number);

        // One mismatch will be a fail condition
        if(FRAM_register.fujID != FRAM_FUJ_ID || FRAM_register.contCode != FRAM_CONT_CODE
                || FRAM_register.prodID1 != FRAM_PROD1 || FRAM_register.prodID2 != FRAM_PROD2)
        {
            chip_health = chip_health_array[chip_number].HealthCount + 1; // Increment value in array by 1 if failed
        }
    }
    else if (chip_number_norm < 12) // MRAM
    {
        // Retrieve the contents of the status register
        uint8_t MRAM_register = MRAMStatusRead(chip_number);

        // Check for an incorrect return
        if ((MRAM_register^MRAM_EXPECTED) != 0)
        {
            chip_health = chip_health_array[chip_number].HealthCount + 1; // Increment value in array by 1 if failed
        }
    }
    else if (chip_number_norm < 16) // SRAM
    {
        // Retrieve the contents of the status register
        uint8_t SRAM_register = SRAMStatusRead(chip_number);

        // Check for an incorrect return
        if ((SRAM_register^SRAM_EXPECTED) != 0)
        {
            chip_health = chip_health_array[chip_number].HealthCount + 1; // Increment value in array by 1 if failed
        }
    }

    // Finally, update the health array with the result
    chip_health_array[chip_number].HealthCount = chip_health;
    if(chip_health > CHIP_HEALTH_MAX)
    {
        // If the chip has failed this check enough times, mark it as dead.
        chip_death_array[chip_number] = 1;
    }

    return chip_health;
}

void
InitializeChipHealth(void)
{
    // Sets all values in the chip health array to be 0.

    uint8_t i;
    for(i = 0; i < MAX_CHIP_NUMBER; i++)
    {
        chip_health_array[i].HealthCount = 0;
        chip_health_array[i].WatchdogCount = 0;
        chip_death_array[i] = 0;
    }
}
