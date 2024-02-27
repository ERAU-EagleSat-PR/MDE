/*
 *  UART0_func.c
 *
 *  Created on: Feb 9, 2024
 *      Author: Tyler
 */

/*
*******************************************************************************
*                                   Includes                                  *
*******************************************************************************
*/

// Standard C files
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware and Driver files
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

// Other Includes
#include "source/devtools.h"
#include "source/UART0_func.h"


//-----------------------------------------------------------------------------
// The UART0 interrupt handler. Debugging UART
//-----------------------------------------------------------------------------
void
UARTDebugIntHandler(void)
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
#ifdef DEBUG
            processDebugInput(local_char);
#endif
        }

    }

}

//-----------------------------------------------------------------------------
// Enable and configure UART0 for debugging
//-----------------------------------------------------------------------------
void
UARTDebugEnable(void)
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
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BAUD_RATE_DEBUG,
                        (UART_CONFIG_WLEN_8 |
                                UART_CONFIG_STOP_ONE |
                                UART_CONFIG_PAR_NONE));

    // Map the Interrupt handler for UART 0
    UARTIntRegister( UART_DEBUG , UARTDebugIntHandler );

    // Enable interrupts for UART 0
    UARTIntEnable( UART_DEBUG, UART_INT_RX | UART_INT_RT);
}


//-----------------------------------------------------------------------------
// Send a string to the Debug UART.
//-----------------------------------------------------------------------------
void
UARTDebugSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
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
