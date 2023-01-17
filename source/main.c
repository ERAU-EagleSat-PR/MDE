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
#include "source/multiplexer.h"
#include "source/obc.h"
#include "source/devtools.h"
#include "source/chips.h"
//#include "source/mde_timer.h"

/*
*******************************************************************************
*                             Global Variables                                *
*******************************************************************************
*/

// Loop variable for blink
volatile uint32_t ui32Loop;

// Variables for the timer
uint32_t timer_current_cycle = 0;
bool timer_wakeup = false;

// The current clock speed
uint32_t SysClkSpeed = 16000000;

// State tracker initial definitions for the menus
#ifdef DEBUG
enum MENU_STATES menu_state          = INIT;
enum BOARDS selectedBoardNumber      = NO_BOARD;
enum MEM_TYPES selectedChipType      = NO_MEM_TYPE;
enum CHIP_NUMBERS selectedChipNumber = NO_CHIP;
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

// LED Functions
void EnableLED(void);
void BlinkRedLED(void);
void BlinkBlueLED(void);
void BlinkGreenLED(void);


//-----------------------------------------------------------------------------
// Enable and configure UART1 for science
//-----------------------------------------------------------------------------
/*
void EnablePrimaryUART(void)
{

}
*/


#ifdef ENABLE_UART_DEBUG


#endif /* ENABLE_UART_DEBUG  */

/*
*******************************************************************************
*                               Chip Select Pins                              *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// Enables GPIO for Board 1 chip select
//-----------------------------------------------------------------------------
void EnableBoard1ChipSelectPins(void)
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
}

//-----------------------------------------------------------------------------
// Enables GPIO for Board 2 chip select
//-----------------------------------------------------------------------------
void EnableBoard2ChipSelectPins(void)
{
    IntMasterDisable();

    //*
    // Enable GPIO for Board 2 chip select
    SysCtlPeripheralEnable(BOARD2_CS_PORT_SYSCTL);

    // Wait to be ready
    while(!SysCtlPeripheralReady(BOARD2_CS_PORT_SYSCTL))
    {
    }

    // Set pins as output
    GPIOPinTypeGPIOOutput(BOARD2_CS_PORT_BASE,
                          CS2_PIN_0 | CS2_PIN_1 | CS2_PIN_2 | CS2_PIN_3 );
    //*/

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
    //*/

    IntMasterEnable();
}

/*
*******************************************************************************
*                                LED FUNCTIONS                                *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// Enables the RGB LED
//-----------------------------------------------------------------------------
void EnableLED(void)
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
void BlinkRedLED(void)
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
void BlinkBlueLED(void)
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
void BlinkGreenLED(void)
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

///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//                                                                            *
//                                      MAIN                                  *
//                                                                            *
//*****************************************************************************
int main(void)
{

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    FPUEnable();
    FPULazyStackingEnable();

    // Set the clock speed.
    SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320,
                       SYS_CLK_SPEED);

    // Enable processor interrupts.
    IntMasterEnable();

    //*****************************
    // Peripheral Enablers
    //*****************************

    EnableLED();

    EnableBoard1ChipSelectPins();

    EnableBoard2ChipSelectPins();


#ifdef DEBUG

    // UART Enable and Configuration
    UARTDebugEnable();

    // Initialize Debug Menu
    menu_state = MAIN;
    printDebugMenu();

#endif

    uint8_t code;
    int chipNum;

    while (1)
    {

#ifdef DEBUG
        BlinkRedLED();
#else   /* Idle "heart beat" */
        BlinkBlueLED();
#endif
        //BlinkGreenLED();


        // currently checking to see if logic is working and if all CS port and
        // pins are active
        /*
        for (chipNum = 0; chipNum <= 15; chipNum++)
        {

            // get mux code to write to CS pins
            code = RetreiveCSCode(chipNum);

            // Write to Board 1 CS pins
            GPIOPinWrite(BOARD1_CS_PORT_BASE,
                         CS1_PIN_0 | CS1_PIN_1 | CS1_PIN_2 | CS1_PIN_3 ,
                         code);

            // Write to Board 2 CS pins
            code = code << 4;
            GPIOPinWrite(GPIO_PORTC_BASE,
                         GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 ,
                         code);

        }
        */

    }

    return 0;
}

//*****************************************************************************
//                                                                            *
//                                END MAIN                                    *
//                                                                            *
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
