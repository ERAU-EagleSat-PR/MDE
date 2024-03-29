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

	// Buffer of the incoming message
	uint32_t uart_obc_msg_chars[UART_OBC_MAX_MSG_SIZE];
	
	//intiialze buffer index to 0
	int uart_obc_msg_index = 0;

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
        if(local_char != -1)
        {
            // TODO : Store the character in the receive buffer

            // Add each incoming character to buffer in hex format
			uart_obc_msg_chars[uart_obc_msg_index] = local_char;
			// Increment the buffer index
			uart_obc_msg_index += 1;
            processOBCInput(local_char);
        }

    }


	// Cast the buffer to uint8_t
	uint8_t *uart_obc_msg_uint8_ptr = (uint8_t *)uart_obc_msg_chars;

	// Call the UARTOBCRecvMsgHandler() function to process the buffer
	UARTOBCRecvMsgHandler(uart_obc_msg_uint8_ptr);


}

//-----------------------------------------------------------------------------
// Enable and configure UART1 for Comms with OBC
//-----------------------------------------------------------------------------
void UARTOBCEnable(void)
{
    //*

    // Enable Debug UART0
    SysCtlPeripheralEnable(UART_OBC_SYSCTL);
    while(!SysCtlPeripheralReady(UART_OBC_SYSCTL))
    {
    }

    // Enable GPIO for OBC coms
    SysCtlPeripheralEnable(UART_OBC_SYSCTL);
    while(!SysCtlPeripheralReady(UART_OBC_SYSCTL));
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
void UARTOBCRecvMsgHandler(uint8_t* incoming_msg_ptr )
{
	/* 
		This function is called when a UART message is received from the OBC
		It will break the message into packets
	*/

	// recover the message length
	int msg_length = sizeof(*incoming_msg_ptr) / sizeof(uint8_t);

	// Iiterate through the buffer
	for (int i = 0; i < msg_length; i++)

		// Check for ESC character
			// signifies start and of message data frame
			// create temp buffer (cmd_msg or something) for data frame
	
			// Check buffer SOM character
				// use SOM to calibrate? check if fist data frame is SOM,
				// if not, send error message to OBC that request was not
				// understood 

			// Check buffer for EOM character
				// break

			// Else, Buffer is command data
				// process command data
				// UARTOBCResponseHandler(cmd_msg);

//*/ 


//-----------------------------------------------------------------------------
// Reponse to UART commands from OBC
//-----------------------------------------------------------------------------

//*
void UARTOBCResponseHandler(void)
{

}
//*/

#endif /* ENABLE_UART_ENABLE */
