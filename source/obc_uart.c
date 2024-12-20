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
#include "source/bit_errors.h"
#include "source/devtools.h"    // Used to write debug code
#include "source/mde.h"         // Used to access the cycle count
#include "source/chip_health.h" // Used to access chip_death_array
#include "source/UART0_func.h"

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

// UART Pin connections (RX and TX refer to MDE RX and TX pins):
// MDE dev board: PB0: RX; PB1: TX
// MDE Stack Header: D10: RX; D11: TX
// OBC Header H2: 50: RX; 49: TX

// UART OBC message buffer - filled by the interrupt handler, and 
// processed by UARTOBCRecvMsgHandler
static uint32_t uart_obc_msg_chars[UART_OBC_MAX_MSG_SIZE];
	
// count tracking the length of the incoming message 
// As of 2023-9-11, this should always be 7, but either way we can iterate
// over the message safely
static int uart_obc_msg_index = 0;

// ID used to differentiate error packets. This value is incremented each time 
// the error data is cleared. So, until OBC clears the health data, the errors
// will be associated with the same health packet. Once data is retrieved by OBC
// and transmitted down to ground station, we can filter out duplicate data if 
// OBC ever sent get health twice before sending clear health.
static uint8_t unique_id = 0;

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
    ui32Status = UARTIntStatus(UART_OBC_UART_PORT_BASE, true);

    //
    // Loop while there are unsigned characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART_OBC_UART_PORT_BASE))
    {
        int32_t local_char;
        local_char = UARTCharGet(UART_OBC_UART_PORT_BASE);

		// Ensure we don't buffer overflow like an idiot
        if(local_char != -1 && uart_obc_msg_index < 64)
        {
            // Add each incoming character to buffer in hex format
			uart_obc_msg_chars[uart_obc_msg_index] = local_char;
			// Increment the buffer index
			uart_obc_msg_index += 1;
        }
    }
#ifdef DEBUG
        int i = 0;
        for(i = 0; i < uart_obc_msg_index; ++i) {
            UARTCharPut(UART_DEBUG, uart_obc_msg_chars[i]);
        }
        UARTCharPut(UART_DEBUG, '\r');
        UARTCharPut(UART_DEBUG, '\n');

#endif /* DEBUG */

	// Figure out if OBC is speaking our language
	// All messages start with an escape character (to indicate that something is coming)
	// followed by the start of message character
    // Finally, an escape character is sent to signal the start of data transmission
	if(uart_obc_msg_chars[0] == UART_OBC_ESCAPE &&
	   uart_obc_msg_chars[1] == UART_OBC_SOM) {
		// The command IDs are kind of stupid, but it's not like we're limited on 
		// transmission ability, and Calvin and Hayden (Rozsell) thought it was funny
		// Can't say I disagree - Nikhil
		if(uart_obc_msg_chars[2] == 'M' &&
		   uart_obc_msg_chars[3] == 'D' && 
           uart_obc_msg_chars[4] == UART_OBC_ESCAPE &&
		   uart_obc_msg_chars[5] == UART_OBC_EOM ) {
			// OBC wants the health and error data, so send it to them

#ifdef DEBUG
            char msg[] = "Return health and error data\r\n";
		    UARTDebugSend((uint8_t*)msg, strlen(msg));
#endif /* DEBUG */

            // Send characters signaling start of message
            UARTCharPut(UART_OBC_UART_PORT_BASE, UART_OBC_ESCAPE);
            UARTCharPut(UART_OBC_UART_PORT_BASE, UART_OBC_SOM);

            // Transmit the health packet (includes prepended ESC)
            TransmitHealth();

            // If we have errors, send them
            if((ErrorQueue_IsEmpty(errorHead) == errorsNotEmpty) && errorHead != NULL){
                TransmitErrors();
            }
            // If not, send a NAK prepended with an escape
            else {
                UARTCharPut(UART_OBC_UART_PORT_BASE, UART_OBC_ESCAPE);
                UARTCharPut(UART_OBC_UART_PORT_BASE, UART_OBC_NAK);
#ifdef DEBUG
            char msg[] = "No error data\r\n";
            UARTDebugSend((uint8_t*)msg, strlen(msg));
#endif /* DEBUG */
            }

            // Send characters signaling end of message
            UARTCharPut(UART_OBC_UART_PORT_BASE, UART_OBC_ESCAPE);
            UARTCharPut(UART_OBC_UART_PORT_BASE, UART_OBC_EOM);
		}
		else if(uart_obc_msg_chars[2] == 'D' &&
		        uart_obc_msg_chars[3] == 'M' && 
                uart_obc_msg_chars[4] == UART_OBC_ESCAPE &&
		        uart_obc_msg_chars[5] == UART_OBC_EOM )  {
			// OBC wants us to clear the health and error data

            // If we're debugging, send output to Debug UART
#ifdef DEBUG
            char debug_msg[] = "Clear health data\r\n";
			UARTDebugSend((uint8_t*)debug_msg, strlen(debug_msg));
#endif /* DEBUG */

            // Clear errors
            ErrorQueue_Destroy(&errorHead);
		}
		memset(uart_obc_msg_chars, 0, 64);
	}

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART_OBC_UART_PORT_BASE, ui32Status);


}

//-----------------------------------------------------------------------------
// Enable and configure UART1 for Comms with OBC
//-----------------------------------------------------------------------------
void UARTOBCEnable(void)
{

    SysCtlPeripheralEnable(UART_OBC_SYSCTL_GPIO);
    SysCtlPeripheralEnable(UART_OBC_SYSCTL_UART);

    // Wait for peripheral access to be ready
    while(!SysCtlPeripheralReady(UART_OBC_SYSCTL_GPIO));
    while(!SysCtlPeripheralReady(UART_OBC_SYSCTL_UART));

    GPIOPinTypeUART(UART_OBC_GPIO_PORT_BASE, UART_OBC_RX_PIN | UART_OBC_TX_PIN);
    GPIOPinConfigure(UART_OBC_RX_PIN_CFG);
    GPIOPinConfigure(UART_OBC_TX_PIN_CFG);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART_OBC_UART_PORT_BASE, 
                        SysCtlClockGet(), UART_OBC_BAUD_RATE,
                        (UART_CONFIG_WLEN_8 | 
                         UART_CONFIG_STOP_ONE | 
                         UART_CONFIG_PAR_NONE));

    // Enable UART Interrupts
    IntEnable(INT_UART_OBC);
    // Enable interrupts for recieve and receive timeout
    UARTIntEnable(UART_OBC_UART_PORT_BASE, UART_INT_RX | UART_INT_RT);

    // Register the interrupt handler for the OBC UART
    UARTIntRegister(UART_OBC_UART_PORT_BASE, UARTOBCIntHandler);
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
    UARTCharPut(UART_OBC_UART_PORT_BASE, *pui8Buffer++);
  }
}

//-----------------------------------------------------------------------------
// Transmit the current error buffer over UART to OBC
//-----------------------------------------------------------------------------
void TransmitErrors()
{
    // Create array to hold data.
	uint8_t error_data[ERROR_DATA_LENGTH];

    // Set the error packet "headers" - the characters that designate this as an error packet
    // These don't change with each packet, so we can just set them outside the loop
    error_data[0] = UART_OBC_ESCAPE;
    error_data[1] = UART_OBC_ERROR_PACKET; // Packet Type ID

#ifdef DEBUG
    UARTDebugSend("Error Data:\r\n", 13);
#endif
    MDE_Error_Data_t *ptr = errorHead;
    if(ptr != NULL){
        do {
            // Take data and fill array - see SDD in the MDE documentation in Sharepoint for a description
            // of each part of the packet
            error_data[2] = unique_id++; // Unique ID
            error_data[3] = ptr->chip_id; // Chip ID 
            error_data[4] = (uint8_t) ((ptr->cell_address >> 24) & 0xFF); // Cell address, includes next 3 bytes
            error_data[5] = (uint8_t) ((ptr->cell_address >> 16) & 0xFF);
            error_data[6] = (uint8_t)((ptr->cell_address >> 8) & 0xFF);
            error_data[7] = (uint8_t) ((ptr->cell_address) & 0xFF);
            error_data[8] = ptr->written_sequence;  // Written Sequence
            error_data[9] = ptr->retrieved_sequence; // Retrieved Sequence

            // Move to the next error in the queue
            ptr = ptr->next;
#ifdef DEBUG
            // Debug message, will contain each character in error_data as
            // a 2 character hex value, with spaces in between
            char debug_msg[ERROR_DATA_LENGTH * 3 + 1];

            int i = 0;
            for(i = 0; i < ERROR_DATA_LENGTH; ++i) {
                // Print every byte in error_data as a 3 character string, with 2 characters for a
                // hex value, and a space afterwards.
                // Have to print 4 characters, since snprintf automatically makes the last character
                // a null terminator. So, we have to overwrite the null terminator on all but the last
                // print. However, it does add the null terminator for us, so that's nice
            snprintf(&debug_msg[i*3], 4, "%02x ", error_data[i]);
            }

            UARTDebugSend((uint8_t*)debug_msg, strlen(debug_msg));
            UARTDebugSend("\r\n", 2);
#endif // DEBUG //

	        // Transmit error packet
	        UARTOBCSend(error_data, ERROR_DATA_LENGTH);
        } while (ptr != errorHead);
    }    
#ifdef DEBUG
    UARTDebugSend("End Error Data\r\n\n", 17);
#endif
}

//-----------------------------------------------------------------------------
// Create the health data packet and transmit it over UART to OBC
//-----------------------------------------------------------------------------
void TransmitHealth(void)
{
    // Create array to hold data.
	uint8_t health_data[HEALTH_DATA_LENGTH];

    // uint32_t to store the chip death data in
    uint32_t health_array = 0;



    memcpy(&health_array, chip_death_array, 4);

    // Assemble Health packet - see SDD in the MDE documentation in Sharepoint for a description
    // of each part of the packet
    health_data[0] = UART_OBC_ESCAPE;   // Escape character - signals that data is being sent
    health_data[1] = UART_OBC_HEALTH_PACKET; // Packet Type ID
    health_data[2] = unique_id++; // Unique ID
    health_data[3] = (uint8_t)( (cycle_count >> 8) & 0xFF); // Cycle count high
    health_data[4] = (uint8_t)( (cycle_count) & 0xFF); // Cycle count low
    health_data[5] = (uint8_t)( (health_array >> 24) & 0xFF); // Health array for idk
    health_data[6] = (uint8_t)( (health_array >> 16) & 0xFF); // Health array for idk
    health_data[7] = (uint8_t)( (health_array >> 8) & 0xFF); // Health array for idk
    health_data[8] = (uint8_t)( (health_array) & 0xFF); // Health array for idk

#ifdef DEBUG
    // iterator variable for for loops
    int i = 0;

    // Debug message, will contain each character in error_data as
    // a 2 character hex value, with spaces in between
    char debug_msg[HEALTH_DATA_LENGTH * 3 + 2];
    for(i = 0; i < HEALTH_DATA_LENGTH; ++i) {
        // Print every byte in error_data as a 3 character string, with 2 characters for a
        // hex value, and a space afterwards.
        // Have to print 4 characters, since snprintf automatically makes the last character
        // a null terminator. So, we have to overwrite the null terminator on all but the last
        // print. However, it does add the null terminator for us, so that's nice
        snprintf(&debug_msg[i*3], 4, "%02x ", health_data[i]);
    }
    UARTDebugSend("\r\nHealth Data:\r\n", 16);
    UARTDebugSend((uint8_t*)debug_msg, strlen(debug_msg));
    UARTDebugSend("\r\nEnd Health Data\r\n\r\n", 21);
#endif // DEBUG //

    // Transmit data
	UARTOBCSend(health_data, HEALTH_DATA_LENGTH);
}

//-----------------------------------------------------------------------------
// Send a packet to OBC requesting a reset (power cycle MDE by turning off the 3.3V rail)
//-----------------------------------------------------------------------------
void MDERequestReset(void) {
	uint8_t msg[] = {UART_OBC_ESCAPE, UART_OBC_SOM, 0x52, 0x80, UART_OBC_ESCAPE, UART_OBC_EOM};
    UARTOBCSend(msg, 5);
}

#ifdef DEBUG

//-----------------------------------------------------------------------------
// Manually fill the message buffer with a given string
//-----------------------------------------------------------------------------
void UARTOBCSetMsg(const uint8_t *pui8Buffer, uint32_t ui32Count) {
    int i = 0;
    for(i = 0; i < ui32Count; ++i) {
        uart_obc_msg_chars[i] = pui8Buffer[i];
    }
    UARTOBCIntHandler();
}

#endif /* DEBUG */

