/*
 * obc.c
 *
 *  Created on: Dec 29, 2022
 *      Author: Calvin
 */

/*
*******************************************************************************
*                             Include Files                                   *
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

#include "source/obc.h"
/*
*******************************************************************************
*                                   OBC Functions                             *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// The UART0 interrupt handler. Debugging UART
//-----------------------------------------------------------------------------
void UARTOBCIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrupt status.
    //
    ui32Status = UARTIntStatus(UART_OBC, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART_OBC, ui32Status);

    //
    // Loop while there are unsigned characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART_OBC))
    {
        int32_t local_char;
        local_char = UARTCharGet(UART_OBC);
        if(local_char != -1)
        {
            processDebugInput(local_char);
        }
    }

}

//-----------------------------------------------------------------------------
// Enable and configure UART0 for debugging
//-----------------------------------------------------------------------------
void UARTOBCnable(void)
{
    /*
    // Enable Debug UART0
    SysCtlPeripheralEnable(UART_OBC_SYSCTL);

    // Wait while un-ready
    while(!SysCtlPeripheralReady(UART_OBC_SYSCTL))
    {
    }

    // Enable GPIO A for Virtual Com Port       //TODO: GPIO for OBC coms
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

    */
}

//-----------------------------------------------------------------------------
// Send a string to the UART.
//-----------------------------------------------------------------------------
void UARTOBCSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
  //
  // Loop while there are more characters to send.
  //
  while(ui32Count--)
  {
    //
    // Write the next character to the UART.
    //
    UARTCharPut(UART_OBC, *pui8Buffer++);
  }
}


//-----------------------------------------------------------------------------
// Transmit the current error buffer over UART to OBC
//-----------------------------------------------------------------------------

/*
void TransmitErrors(){

  uint32_t error_iter;
  unsigned char data1;
  unsigned char data2;
  unsigned char data3;
  for(error_iter = 0; error_iter < current_error; error_iter++){

    while(UARTBusy(UART_PRIMARY)){
    }

    // Transmit the 3 byte data header, 0b10000000/0x80
    UARTCharPut(UART_PRIMARY, 0x80);
    UARTCharPut(UART_PRIMARY, 0x80);
    UARTCharPut(UART_PRIMARY, 0x80);

    // Putting data into uchars for transmission
    data1 = error_buffer[error_iter] & 0xFF;
    data2 = (error_buffer[error_iter]>>8) & 0xFF;
    data3 = (error_buffer[error_iter]>>16) & 0xFF;

    // Transmit data
    UARTCharPut(UART_PRIMARY, data3);
    UARTCharPut(UART_PRIMARY, data2);
    UARTCharPut(UART_PRIMARY, data1);

  }
  current_error = 0;

}

*/

//-----------------------------------------------------------------------------
// Create the health data integer and transmit it over UART to OBC
//-----------------------------------------------------------------------------
/*
void TransmitHealth(){

  uint64_t health_data;

  //Heath_data is a mask
  health_data = (((uint64_t)HEALTH_DATA_HEADER_VALUE & HEALTH_DATA_HEADER_MASK) << HEALTH_DATA_HEADER_SHIFT) |
    (((uint64_t)cycle_count & HEALTH_DATA_CYCLE_MASK) << HEALTH_DATA_CYCLE_SHIFT) |
      (((uint64_t)chip_unresponsive & HEALTH_DATA_RESPONSIVENESS_MASK) << HEALTH_DATA_RESPONSIVENESS_SHIFT) |
        (((uint64_t)current_sequence_start & HEALTH_DATA_START_MASK) << HEALTH_DATA_START_SHIFT) |
          (((uint64_t)current_sequence_offset & HEALTH_DATA_OFFSET_MASK) << HEALTH_DATA_OFFSET_SHIFT);

    #ifdef DEBUG

  char buf[100];
  sprintf(buf, "Health Data: %llx", health_data);
  UARTSend((uint8_t*) buf, strlen(buf));

#endif

  unsigned char data0;
  unsigned char data1;
  unsigned char data2;
  unsigned char data3;
  unsigned char data4;
  unsigned char data5;
  unsigned char data6;
  unsigned char data7;

  // Putting data into uchars for transmission
  data0 = health_data & 0xFF;
  data1 = (health_data>>8) & 0xFF;
  data2 = (health_data>>16) & 0xFF;
  data3 = (health_data>>24) & 0xFF;
  data4 = (health_data>>32) & 0xFF;
  data5 = (health_data>>40) & 0xFF;
  data6 = (health_data>>48) & 0xFF;
  data7 = (health_data>>54) & 0xFF;

  // Transmit data
  UARTCharPut(UART_PRIMARY, data7);
  UARTCharPut(UART_PRIMARY, data6);
  UARTCharPut(UART_PRIMARY, data5);
  UARTCharPut(UART_PRIMARY, data4);
  UARTCharPut(UART_PRIMARY, data3);
  UARTCharPut(UART_PRIMARY, data2);
  UARTCharPut(UART_PRIMARY, data1);
  UARTCharPut(UART_PRIMARY, data0);

}
*/

