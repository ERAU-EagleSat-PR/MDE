/*
 * Main File for the EagleSat-2 Memory Degredation Experiment
 */

/*
 *******************************************************************************
 *                               Include Files                                 *
 *******************************************************************************
 */

// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware Files
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

// Driver Files
#include <driverlib/debug.h>
#include <driverlib/fpu.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/rom.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>

// Custom Header Files
#include "source/mde.h"
#include "source/chips.h"
#include "source/chip_health.h"
#include "source/bit_errors.h"
#include "source/multiplexer.h"
#include "source/obc_uart.h"
#include "source/mde_timers.h"
#include "source/UART0_func.h"
#include "source/devtools.h"


// Chip drivers
#include "source/chipDrivers/FLASHfunc.h"
#include "source/chipDrivers/MRAMfunc.h"
#include "source/chipDrivers/SRAMfunc.h"
#include "source/chipDrivers/FRAMfunc.h"

/*
 *******************************************************************************
 *                        Initialize Global Variables                          *
 *******************************************************************************
 */

// Variables for the timer
bool timer_wakeup = false;          // unnecessary?
bool reading_chip = 0;
uint32_t cycle_time_clockrate = (uint32_t)MEMORY_CYCLE_TIME * (uint32_t)MINUTE * (uint32_t)SYS_CLK_SPEED;
uint32_t timer_current_cycle = 0;  // Maximum timer value is limited by the 32 bit architecture, so we must complete multiple timer interrupts before processing.
uint32_t wd_chip_time = (uint32_t)CHIP_WD_TIME * (uint32_t)MINUTE * (uint32_t)SYS_CLK_SPEED;
uint32_t wd_mde_time =  (uint32_t)SYS_CLK_SPEED;// * (uint32_t)4 * (uint32_t)MINUTE;

volatile uint32_t ui32Loop;         // Loop variable for blink

// Error Variables
uint32_t current_error = 0;         // global error count (from bit_errors.c)
MDE_Error_Data_t *errorHead = 0;

// Global Chip Trackers
uint8_t auto_chip_number = 0;       // Chip tracker for auto mode (from mde.h)
uint8_t current_chip = 17;           // track active chip (from chips.h)
CHIPHEALTH chip_health_array[32];   // track chip health (from chiphealth.h)
bool chip_death_array[32];          // track dead chips (from chiphealth.h)
bool reading_chip;                  // read/write tracker (from mde_timers.h)

// Cycle count - global tracker for the number of times MDE has gone through the main
// experiment loop
uint16_t cycle_count = 0;
uint8_t currentCycle;               // Value 0 or 255 for writing all 0s or 1s

// Debug Variables (from devtools.h)
#ifdef DEBUG
enum MENU_STATES menuState  = INIT; // Debug menu state
enum BOARDS selectedBoardNumber = BOARD2; // Selected Board for debug
uint8_t chipSelectStep = 1;         // Used for chip type -> chip number step tracking
uint8_t seedErrors = 0;             // Value 0 or 1 for seeding errors when writing
#endif


/*
 *******************************************************************************
 *                             Function Prototypes                             *
 *******************************************************************************
 */

// UART Functions
/*
void EnablePrimaryUART(void);
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UART0IntHandler(void);
void EnableDebugUART(void);
void UARTSendDebug(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UARTSendDebug(const uint8_t *pui8Buffer, uint32_t ui32Count);
void printMenu(void);
void processDebugInput(int32_t recv_char);
void processMainMenuInput(int32_t recv_char);
void processCSBoardMenuInput(int32_t recv_char);
void processCSTypeMenuInput(int32_t recv_char);
void processCSNumMenuInput(int32_t recv_char);
 */

#ifdef DEBUG
// LED Functions
void EnableLED(void);
void BlinkRedLED(void);
void BlinkBlueLED(void);
void BlinkGreenLED(void);
#endif

//-----------------------------------------------------------------------------
// Enable and configure UART1 for science
//-----------------------------------------------------------------------------
/*
void EnablePrimaryUART(void)
{

}
 */


/*
 *******************************************************************************
 *                               Chip Select Pins                              *
 *******************************************************************************
 */

//-----------------------------------------------------------------------------
// Enables GPIO for Board 1 chip select
//-----------------------------------------------------------------------------
void
EnableBoard1ChipSelectPins(void)
{
    IntMasterDisable();

    // Enable GPIO
    SysCtlPeripheralEnable(BOARD1_CS_PORT_SYSCTL);

    // wait to be ready
    while(!SysCtlPeripheralReady(BOARD1_CS_PORT_SYSCTL))
    {
    }

    //Set pins as output
    GPIOPinTypeGPIOOutput(BOARD1_CS_PORT_BASE,
                          CS1_PIN_0 | CS1_PIN_1 | CS1_PIN_2 | CS1_PIN_3 );

    IntMasterEnable();
    // Reset pins
    ResetChipSelect1();
}

//-----------------------------------------------------------------------------
// Enables GPIO for Board 2 chip select
//-----------------------------------------------------------------------------
void
EnableBoard2ChipSelectPins(void)
{
    IntMasterDisable();


    // Enable GPIO for Board 2 chip select
    SysCtlPeripheralEnable(BOARD2_CS_PORT_SYSCTL);

    // Wait to be ready
    while(!SysCtlPeripheralReady(BOARD2_CS_PORT_SYSCTL))
    {
    }

    // Set pins as output
    GPIOPinTypeGPIOOutput(BOARD2_CS_PORT_BASE, CS2_PIN_0 | CS2_PIN_1 | CS2_PIN_2 | CS2_PIN_3 );


    /*
    // Enable GPIO for Board 2 chip select
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Wait to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
    {
    }

    // Set pins as output
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |GPIO_PIN_7 );


    IntMasterEnable();
    // Reset pins
    ResetChipSelect2();*/
}

/*
 ******************************************************************************
 *                                LED FUNCTIONS                               *
 ******************************************************************************
 */


//-----------------------------------------------------------------------------
// Enables the RGB LED
//-----------------------------------------------------------------------------
void
EnableLED(void)
{

    // Enable LED GPIO PIN
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Check for peripheral to be Enabled
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

  // Enable the GPIO pin for the LEDs (PF1 - Red, PF2 - Blue, PF3 - Green).  Set the direction as output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);


}

//-----------------------------------------------------------------------------
// Blinks the Red LED
//-----------------------------------------------------------------------------
void
BlinkRedLED(void)
{

    // Turn on RED LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

    // Delay for a bit.
    for(ui32Loop = 0; ui32Loop < 20; ui32Loop++)
    {
    }

    // Turn off GREEN LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 10; ui32Loop++)
    {
    }
}

//-----------------------------------------------------------------------------
// Blinks the Blue LED, Signifying the MDE is IDLE / WAITING
//-----------------------------------------------------------------------------
void
BlinkBlueLED(void)
{

    // Turn on BLUE LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

    // Delay for a bit.
    for(ui32Loop = 0; ui32Loop < 10; ui32Loop++)
    {
    }

    // Turn off BLUE LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 10; ui32Loop++)
    {
    }
}

//-----------------------------------------------------------------------------
// Blinks the Green LED
//-----------------------------------------------------------------------------
void
BlinkGreenLED(void)
{

    // Turn on GREEN LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

    // Delay for a bit.
    for(ui32Loop = 0; ui32Loop < 2; ui32Loop++)
    {
    }

    // Turn off GREEN LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 100; ui32Loop++)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//                                                                            *
//                                      MAIN                                  *
//                                                                            *
//*****************************************************************************
int
main(void)
{

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    FPUEnable();
    FPULazyStackingEnable();

    // Set the clock speed.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    // Disable interrupts during startup
    IntMasterDisable();

    // Startup light. This has to be before EnableSPI() as the SPI for Board 2 (SSI1) uses
    // the GPIO Port F (pins 0-2)
    EnableLED();

    // Turn on BLUE LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

    uint16_t k;
    // Delay for a bit.
    for(k = 0; k < 5000; k++)
    {
    }

    // Turn off BLUE LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);

    //*****************************
    // Peripheral Enablers
    //*****************************

    MDETimerConfigure();

    MDETimerDisable();

    EnableSPI(); // Chip SPI communications

    EnableBoard1ChipSelectPins(); // Board 1 MUX enable

    EnableBoard2ChipSelectPins(); // Board 2 MUX enable

    BoardPowerInit(); // Initialize the Pins controlling the board power
    // includes turning power off

    Board1PowerOn();
    Board2PowerOn();



    UARTDebugEnable();
    // Set up the debug menu if debugging is enabled
#ifdef DEBUG
    char buf[60] = "Wait until debug menu appears before doing anything\r\n";
    uint8_t bufSize = 60;
    // UART Enable and Configuration
    UARTCharPut(UART_DEBUG, 0xC);
    UARTDebugSend((uint8_t*)buf, strlen(buf));
#endif
    //*****************************
    // Chip Configurations
    //*****************************

    // Initialize Health and Death arrays
    InitializeChipHealth();
    // Check all chips before program start
    uint8_t chip;
    uint8_t norm;

    for(chip = 0; chip < MAX_CHIP_NUMBER; chip++)
    {
        norm = chip % 16;

        if(norm < 4)
        {
            FlashConfiguration(chip);
        }
        else if (norm >= 8 && norm < 12) // If the chip is MRAM, prepare its status register
        {
            MRAMStatusPrepare(chip);
        }

#ifdef DEBUG
        snprintf(buf, bufSize,  "Check chip %d...",chip);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif

        CheckChipHealth(chip); // Check health of all chips. This will also initialize chip health array to 1s assuming they are all working.

#ifdef DEBUG
        snprintf(buf, bufSize, "%d\r\n",chip_health_array[chip].HealthCount);
        UARTDebugSend((uint8_t*)buf, strlen(buf));
#endif
    }

    // Initialize all data to 1s, and begin on a 0 cycle.
    for(chip = 0; chip < MAX_CHIP_NUMBER; chip++)
    {
#ifdef DEBUG
        snprintf(buf, bufSize, "Write to chip %d....", chip);
        UARTDebugSend((uint8_t*)buf, strlen(buf));
#endif
        WriteToChip(255,chip);
#ifdef DEBUG
        snprintf(buf, bufSize,  "Done\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
    }

    //*****************************
    // Enable the UART for OBC
    // which is UART1. UART0 is
    // used for debug. So, if debugging,
    // UART output will be on both UARTs
    //*****************************
    UARTOBCEnable();

    currentCycle = 0;

    // Send the debug menu if debugging is enabled
    // We wait to send the debug menu until the chips are written to so that the user knows
    // They cant do anything until then
#ifdef DEBUG

    //WatchdogsEnable(); //TODO Watchdog Timers WHEN DEBUG MODE IS OFF THIS REQUIRES THAT UART0 IS STILL ENABLED
    // Initialize Debug Menu
    menuState = MAIN;
    printDebugMenu();

#else /* DEBUG */
    // Enable flight mode
    MDETimerEnable();
#endif
    MDETimerEnable();

    //*****************************
    // Other Configurations
    //*****************************
    // Enable processor interrupts.
    IntMasterEnable();


    //*****************************
    // Main Loop
    //*****************************
    while (1)
    {
        if(timer_current_cycle >= MEMORY_CYCLE_COUNT)
        {
#ifdef TIMER_DEBUG
            snprintf(buf,bufSize, "Timer Trigger\n\r");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
            // Call the function to begin a new cycle.
            current_chip = 0;
            MDEProcessCycle();
            timer_current_cycle = 0;
        }

        // currently checking to see if logic is working and if all CS port and
        // pins are active
        /*
        for (chipNum = 1; chipNum <= TOTAL_CHIP_COUNT; chipNum++)
        {

            SetChipSelect(chipNum);

        }
        // */

    }

}
// We need to send the first address of the current 256 byte block
//*****************************************************************************
//                                                                            *
//                                END MAIN                                    *
//                                                                            *
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
