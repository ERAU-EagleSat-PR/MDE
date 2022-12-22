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
#include "source/tm4c123gxl_system.h"

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

// State trackers for the menu
enum MENU_STATES menu_state = INIT;
uint32_t selected_chip_number = 0;

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/

// UART Functions
void EnablePrimaryUART(void);
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UART0IntHandler(void);
void EnableDebugUART(void);
void UARTSendDebug(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UARTSendDebug(const uint8_t *pui8Buffer, uint32_t ui32Count);
void printMenu(void);
void processDebugMenu(int32_t recv_char);

// LED Functions
void EnableLED(void);
void BlinkRedLED(void);
void BlinkBlueLED(void);
void BlinkGreenLED(void);


/*
*******************************************************************************
*                               UART FUNCTIONS                                *
*******************************************************************************
*/


//-----------------------------------------------------------------------------
// Enable and configure UART1 for science
//-----------------------------------------------------------------------------
void EnablePrimaryUART(void)
{

}


//-----------------------------------------------------------------------------
// Send a string to the Primary UART.
//-----------------------------------------------------------------------------
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(UART_PRIMARY, *pui8Buffer++);
    }
}


#ifdef ENABLE_UART_DEBUG //////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// The UART0 interrupt handler. Debugging UART
//-----------------------------------------------------------------------------
void UART0IntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrupt status.
    //
    ui32Status = UARTIntStatus(UART_DEBUG, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART_DEBUG, ui32Status);

    //
    // Loop while there are unsigned characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART_DEBUG))
    {
        int32_t local_char;
        local_char = UARTCharGet(UART_DEBUG);
        if(local_char != -1)
        {
            processDebugMenu(local_char);
        }
    }

}

//-----------------------------------------------------------------------------
// Enable and configure UART0 for debugging
//-----------------------------------------------------------------------------
void EnableDebugUART(void)
{
    // Enable Debug UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Wait while un-ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    // Enable GPIO A for Virtual Com Port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Wait while un-ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                       (UART_CONFIG_WLEN_8 |
                        UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE));

    // Map the Interrupt handler for UART 0
    UARTIntRegister( UART_DEBUG , UART0IntHandler );

    // Enable interrupts for UART 0
    UARTIntEnable( UART_DEBUG, UART_INT_RX | UART_INT_RT);
}


//-----------------------------------------------------------------------------
// Send a string to the Debug UART.
//-----------------------------------------------------------------------------
void UARTDebugSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(UART_DEBUG, *pui8Buffer++);
    }
}


//-----------------------------------------------------------------------------
// Print the debug/ development menu to the console
//-----------------------------------------------------------------------------
void printMenu(void)
{

    char buf[200];

    switch (menu_state)
    {
    case INIT:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf,"MDE Debug and Development Interface\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case MAIN:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf, "Menu Selection:\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "M - Return to this menu.\n\r");                   // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "C - Clear screen.\n\r");                          // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "H - Send Health Packet (TransmitHealth) \n\r");   // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "E - Send Data Packet (TransmitErrors) \n\r");     // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "X - Restart Program.\n\r");                       // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case AUTO:
        sprintf(buf,"AUTO MODE. Hit M to exit.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    default:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf,"State failure!");
    }

}

//-----------------------------------------------------------------------------
// Process input from the debug menu
//-----------------------------------------------------------------------------
void processDebugMenu(int32_t recv_char)
{

    char buf[200];

    switch (menu_state)
    {
    case MAIN:
        switch (recv_char)
        {
        case 'm':   // Stay on Main Menu
            menu_state = MAIN;
            break;
        case 'c':   // Clear the screen
            UARTCharPut(UART_DEBUG, 0xC);
            break;
        case 'a':   // Enter Auto Mode
            //menu_state = AUTO;
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        case 'h':
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        case 'e':
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        case 'x':
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        break;
    case AUTO:
        switch (recv_char)
        {
        case 'm':
            menu_state = MAIN;
            break;
        }
        break;
    }

}

#endif ////////////////////////////////////////////////////////////////////////


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
    // extra stack usage.
    FPUEnable();
    FPULazyStackingEnable();

    // Set the clock speed.
    SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320, SYS_CLK_SPEED);

    // Enable processor interrupts.
    IntMasterEnable();

    //*****************************
    // LED enable and configuration
    //*****************************
    EnableLED();

#ifdef DEBUG
    //******************************
    // UART Enable and Configuration
    //******************************
    EnableDebugUART();
#endif

#ifdef DEBUG

    menu_state = MAIN;
    printMenu();

#endif


    while (1)
    {
        // Idle "heart beat"
        BlinkBlueLED();
        BlinkRedLED();
        BlinkGreenLED();

    }

    return 0;
}

//*****************************************************************************
//                                                                            *
//                                END MAIN                                    *
//                                                                            *
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
