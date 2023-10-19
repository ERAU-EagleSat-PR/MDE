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
#include "source/devtools.h"

/*
*******************************************************************************
*                        Initialize Global Variables                          *
*******************************************************************************
*/

// Variables for the timer
uint32_t timer_current_cycle = 0;   // unnecessary?
bool timer_wakeup = false;          // unnecessary?
uint32_t cycle_time_clockrate = MEMORY_CYCLE_TIME*60*SYS_CLK_SPEED;

// Error Variables
uint32_t current_error = 0;         // global error count (from bit_errors.c)

// Global Chip Trackers
uint8_t auto_chip_number = 0;       // Chip tracker for auto mode (from mde.h)
uint8_t current_chip = 5;           // track active chip (from chips.h)
CHIPHEALTH chip_health_array[32];   // track chip health (from chiphealth.h)
MDE_Error_Data                      // catalogue errors  (from bit_errors.h)
*error_buffer[ERROR_BUFFER_MAX_SIZE];
bool chip_death_array[32];          // track dead chips (from chiphealth.h)
bool reading_chip;                  // read/write tracker (from mde_timers.h)

// Cycle count - global tracker for the number of times MDE has gone through the main
// experiment loop
uint16_t cycle_count = 0;

// Debug Variables (from devtools.h)
#ifdef DEBUG
enum MENU_STATES menuState  = INIT; // Debug menu state
uint8_t selectedBoard = 1;          // Value 0 or 1 as an offset
uint8_t currentCycle = 255;           // Value 0 or 255 for writing all 0s or 1s
uint8_t chipSelectStep = 1;         // Used for chip type -> chip number step tracking
uint8_t seedErrors = 0;             // Value 0 or 1 for seeding errors when writing
volatile uint32_t ui32Loop;         // Loop variable for blink
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
#ifdef DEBUG

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

  // Enable the GPIO pin for the RED LED (PF1).  Set the direction as output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

  // Enable the GPIO pin for the GREEN LED (PF3).  Set the direction as output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

  // Enable the GPIO pin for the BLUE LED (PF2).  Set the direction as output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

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
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }

    // Turn off RED LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
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
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }

    // Turn off BLUE LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
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
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }

    // Turn off GREEN LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }
}
#endif
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

    // Disable Interrupts while we're setting stuff up
    IntMasterDisable();

    //*****************************
    // Peripheral Enablers
    //*****************************

    //MDEWatchdogsEnable(); // Watchdog Timers

    MDETimerConfigure();

    EnableSPI(); // Chip SPI communications

    EnableBoard1ChipSelectPins(); // Board 1 MUX enable

    //EnableBoard2ChipSelectPins(); // Board 2 MUX enable

#ifdef DEBUG
    EnableLED(); // Debug LEDs

    // UART Enable and Configuration
    UARTDebugEnable();

    // Initialize Debug Menu
    menuState = MAIN;
    printDebugMenu();

#endif /* DEBUG */

    //*****************************
    // Enable the UART for OBC 
    // which is UART1. UART0 is
    // used for debug. So, if debugging, 
    // UART output will be on both UARTs
    //*****************************
    UARTOBCEnable();

    //*****************************
    // Chip Configurations
    //*****************************
    // Initialize Health and Death arrays
    InitializeChipHealth();

    // Check all chips before program start
    uint8_t chip;
    for(chip = 0; chip < MAX_CHIP_NUMBER; chip++)
    {
        if ((chip % 16) >= 8 && (chip % 16) < 12) // If the chip is MRAM, prepare its status register
        {
            //MRAMStatusPrepare(chip);
        }
        CheckChipHealth(chip); // Check health of all chips. This will also initialize chip health array to 1s assuming they are all working.
    }

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
        #ifdef DEBUG
            BlinkGreenLED();
        #else   /* Idle "heart beat" */
            BlinkRedLED();
        #endif /* DEBUG */

        if(UARTOBCIsDataReady())
            UARTOBCRecvMsgHandler();
        //BlinkGreenLED();


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

//*****************************************************************************
//                                                                            *
//                                END MAIN                                    *
//                                                                            *
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
