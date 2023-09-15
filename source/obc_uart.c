/*
 * obc.c
 *
 *  Created on: Dec 29, 2022
 *      Author: Calvin
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

// local project files
#include "source/obc_uart.h"
#include "source/devtools.h"

#ifdef ENABLE_UART_OBC

/*
*******************************************************************************
*						  OBC UART Variables								  *
*******************************************************************************
*/
// These are variables that need to be accessed by more than one function in
// this file (technically, just the interrupt handler and the receive message)
// handler. The variables are declared as static to make it clear that only
// functions handling MDE-OBC communication (i.e. functions in this file)
// should be able to access them

// UART OBC message buffer - filled by the interrupt handler, and 
// processed by UARTOBCRecvMsgHandler
static uint32_t uart_obc_msg_chars[UART_OBC_MAX_MSG_SIZE];
	
// count tracking the length of the incoming message 
// As of 2023-9-11, this should always be 7, but either way we can iterate
// over the message safely
static int uart_obc_msg_index = 0;

// Boolean used by the interrupt handler to tell the main loop that OBC just 
// sent us a message. Set to true at the end of the interrupt handler and set 
// to false in the message handler (OBCUARTRecvMsgHandler). 
static bool uart_obc_data_ready = false;
/*
*******************************************************************************
*                         OBC UART Packet Structures                          *
*******************************************************************************
*/
typedef struct {
    uint8_t unique_id;
    uint16_t cycle_count;
    uint32_t health_array;
} MDE_Health_Packet_struct;

typedef struct {
    uint8_t unique_id;
    uint8_t chip_id;
    uint16_t cell_address;
    uint8_t written_sequence;
    uint8_t retrieved_sequence;
} MDE_Error_Packet_struct;
/*
*******************************************************************************
*                              OBC UART Functions                             *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// The UART1 (OBC coms) interrupt handler.
//-----------------------------------------------------------------------------
void UARTOBCIntHandler(void)
{
    uint32_t ui32Status;

	// Reset buffer index to 0
	uart_obc_msg_index = 0;

    //
    // Get the interrupt status.
    //
    ui32Status = UARTIntStatus(UART_OBC_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART_OBC_BASE, ui32Status);

    //
    // Loop while there are unsigned characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART_OBC_BASE))
    {
        int32_t local_char;
        local_char = UARTCharGet(UART_OBC_BASE);

		// Ensure we don't buffer overflow like an idiot
        if(local_char != -1 && uart_obc_msg_index < 64)
        {
            // TODO : Store the character in the receive buffer

            // Add each incoming character to buffer in hex format
			uart_obc_msg_chars[uart_obc_msg_index] = local_char;
			// Increment the buffer index
			uart_obc_msg_index += 1;
        }
    }
	uart_obc_data_ready = true;
}

//-----------------------------------------------------------------------------
// Enable and configure UART1 for Comms with OBC
//-----------------------------------------------------------------------------
void UARTOBCEnable(void)
{
    // Enable GPIO for OBC coms
    SysCtlPeripheralEnable(UART_OBC_GPIO_SYSCTL);
    while(!SysCtlPeripheralReady(UART_OBC_GPIO_SYSCTL));
    {
    }

    // Enable OBC UART Base, UART 1
    SysCtlPeripheralEnable(UART_OBC_UART_SYSCTL);
    while(!SysCtlPeripheralReady(UART_OBC_UART_SYSCTL))
    {
    }

    // Configure the UART pins for OBC coms
    GPIOPinTypeUART(UART_OBC_BASE, UART_OBC_RX_PIN |  UART_OBC_TX_PIN);
    GPIOPinConfigure(UART_OBC_RX_PIN_CFG);
    GPIOPinConfigure(UART_OBC_TX_PIN_CFG);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART_OBC_BASE, SysCtlClockGet(), UART_OBC_BAUD_RATE,
                       (UART_CONFIG_WLEN_8 |
                        UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE));

    // Map the Interrupt handler for OBC coms UART
    UARTIntRegister( UART_OBC_BASE , UARTOBCIntHandler );

    // Enable interrupts for OBC coms UART
    UARTIntEnable( UART_OBC_BASE, UART_INT_RX | UART_INT_RT);

    //*/
}

//-----------------------------------------------------------------------------
// Send a string to the UART.
//-----------------------------------------------------------------------------
//TODO
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
    UARTCharPut(UART_OBC_BASE, *pui8Buffer++);
  }
}

//-----------------------------------------------------------------------------
// Format Data in Error Buffer to be trasmitted over UART
//-----------------------------------------------------------------------------
//TODO
void FormatErrorDataPacket() /* Probably wont return a void*/
{
	
}

//-----------------------------------------------------------------------------
// Transmit the current error buffer over UART to OBC
//-----------------------------------------------------------------------------
//TODO
void TransmitErrors()
{
	/*

    Dont forget to use a free(*pointerInBuffer) on every iteration
    so that malloc can continue adding errors -Tyler

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

  	current_error = 0; keep, used for iterating through error buf
	}
	
    */


}

//-----------------------------------------------------------------------------
// Format Health data to be transmitted over UART
//-----------------------------------------------------------------------------
//TODO
void FormatHealthDataPacket() /* Probably wont return a void*/
{
	
}

//-----------------------------------------------------------------------------
// Create the health data integer and transmit it over UART to OBC
//-----------------------------------------------------------------------------
//TODO
void TransmitHealth()
{
	/*
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

#endif // DEBUG //

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

	*/
}

//-----------------------------------------------------------------------------
// Break the incoming UART data into packets for processing/ response
//-----------------------------------------------------------------------------

//*
void UARTOBCRecvMsgHandler(void)
{
	// Figure out if OBC is speaking our language
	// All messages start with an escape character (to indicate that something is coming)
	// followed by the start of message character
	// Then, an escape character is sent to signify the start of data transmission
	if(uart_obc_msg_chars[0] == UART_OBC_ESCAPE &&
	   uart_obc_msg_chars[1] == UART_OBC_SOM &&
	   uart_obc_msg_chars[2] == UART_OBC_ESCAPE) {
		// The command IDs are kind of stupid, but it's not like we're limited on 
		// transmission ability, and Calvin and Hayden (Rozsell) thought it was funny
		// Can't say I disagree - Nikhil
		if(uart_obc_msg_chars[3] == 'M' &&
		   uart_obc_msg_chars[4] == 'D') {
			// OBC wants the health and error data, so send it to them
            char msg[] = "it works lol\r\n";
			UARTDebugSend(msg, strlen(msg));
		}
		else if(uart_obc_msg_chars[3] == 'D' &&
		   uart_obc_msg_chars[4] == 'M') {
			// OBC wants us to clear the health and error data
		}
		else {
			// Tell OBC what they said makes no sense, but they were speaking our language
			// (i.e. error, but different than below)
		}
	}
	else {
		// Tell OBC they aren't speaking our language
		// (i.e. error saying what they sent doesn't match the protocol)
	}
	// Set the data_ready variable to false
	// The message has been processed, so there's no need to check it again
	uart_obc_data_ready = false;

}
//*/ 


//-----------------------------------------------------------------------------
// Reponse to UART commands from OBC
//-----------------------------------------------------------------------------

//*
void UARTOBCResponseHandler(void)
{

}
//*/

bool UARTOBCIsDataReady() {
    return uart_obc_data_ready;
}

#ifdef DEBUG
void UARTOBCSetMsg(const uint8_t *pui8Buffer, uint32_t ui32Count) {
    int i = 0;
    for(i = 0; i < ui32Count; ++i) {
        uart_obc_msg_chars[i] = pui8Buffer[i];
    }
    uart_obc_data_ready = true;
}

#endif /* DEBUG */

#endif /* ENABLE_UART_OBC */
