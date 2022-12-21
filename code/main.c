#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//driver imports
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

// Custom Imports
#include "tm4c123gxl_system.h"
#include "mde.h"

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
uint32_t SysClkSpeed = 0;

// State trackers for the menu
enum MENU_STATES menu_state = INIT;
uint32_t selected_chip_number = 0;


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


#ifdef ENABLE_UART_DEBUG

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

    // Map the Interrupt handler for UART 0
    UARTIntRegister( UART0_BASE , UART0IntHandler );

    // Enable interrupts for UART 0
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}


//-----------------------------------------------------------------------------
// Send a string to the Debug UART.
//-----------------------------------------------------------------------------
void UARTSendDebug(const uint8_t *pui8Buffer, uint32_t ui32Count)
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

#endif


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
        UARTSendDebug((uint8_t*) buf, strlen(buf));
        break;
    case MAIN:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf,"Menu Selection:\n\r");
        UARTSend((uint8_t*) buf, strlen(buf));
        sprintf(buf,"M - Return to this menu.\n\r");
        UARTSend((uint8_t*) buf, strlen(buf));
        sprintf(buf,"I - Enter Auto mode.\n\r");
        UARTSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "H - Send Health Packet (TransmitHealth) \n\r");
        UARTSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "D - Send Data Packet (TransmitErrors) \n\r");
        UARTSend((uint8_t*) buf, strlen(buf));
        break;
    case AUTO:
        sprintf(buf,"AUTO MODE. Hit M to exit.\n\r");
        UARTSend((uint8_t*) buf, strlen(buf));
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
    /*
    char buf[200];

    switch (menu_state)
    {
    case MAIN:
        switch (recv_char)
        {
        case 'm':
            menu_state = MAIN;
            break;
        case 'a':
            menu_state = AUTO;
            UARTCharPut(UART_PRIMARY, 0xC);
            break;
        case '1':
            selected_chip_number = 0;
            ChipSelect(selected_chip_number);
            break;
        case '2':
            selected_chip_number = 1;
            ChipSelect(selected_chip_number);
            break;
        case '3':
            selected_chip_number = 2;
            ChipSelect(selected_chip_number);
            break;
        case '4':
            selected_chip_number = 3;
            ChipSelect(selected_chip_number);
            break;
        case '5':
            selected_chip_number = 4;
            ChipSelect(selected_chip_number);
            break;
        case '6':
            selected_chip_number = 5;
            ChipSelect(selected_chip_number);
            break;
        case '7':
            selected_chip_number = 6;
            ChipSelect(selected_chip_number);
            break;
        case '8':
            selected_chip_number = 7;
            ChipSelect(selected_chip_number);
            break;
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
    */
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

    //******************************
    // UART Enable and Configuration
    //******************************

    #ifdef DEBUG


    #endif

#ifdef DEBUG

    printMenu();

    menu_state = MAIN;
    printMenu();

#endif

    while (1)
    {
        // Idle "heart beat"
        BlinkBlueLED();
    }

    return 0;
}
