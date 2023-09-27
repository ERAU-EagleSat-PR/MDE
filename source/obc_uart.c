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
    // Create array to hold data. Add an extra character for a null
    // terminator so we can call strlen on it
	char error_data[ERROR_DATA_LENGTH + 1];

    /*
     * For actual implementation, we'll have to iterate over the list of errors 
     * and retrieve the information for each error
     */

    // Temporary hard-coded error packet
    error_data[0] = UART_OBC_ESCAPE;
    error_data[1] = UART_OBC_ERROR_PACKET; // Packet Type ID
    error_data[2] = 1; // Unique ID
    error_data[3] = 6; // Chip ID 
    error_data[4] = 9; // Cell address, includes next 3 bytes
    error_data[5] = 4; 
    error_data[6] = 2;
    error_data[7] = 0;
    error_data[8] = 8;  // Written Sequence
    error_data[9] = 9; // Retrieved Sequence
    error_data[10] = '\0'; // Null terminator, used so that we can call strlen for the length
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

        UARTDebugSend("\r\nError Data:\r\n", 15);
	    UARTDebugSend(debug_msg, strlen(debug_msg));
    #endif // DEBUG //

	// Transmit data
	UARTOBCSend(error_data, strlen(error_data));


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
    // Create array to hold data. Add an extra character for a null
    // terminator so we can call strlen on it
	char health_data[HEALTH_DATA_LENGTH + 1];

    /*
    Get cycle count:
    cycle_count = (some function idk)

    get health array
    uint32_t health_array = ????
    */

    // Temporary hard-coded health packet

    health_data[0] = UART_OBC_ESCAPE;
    health_data[1] = UART_OBC_HEALTH_PACKET; // Packet Type ID
    health_data[2] = 1; // Unique ID
    health_data[3] = 6; // Cycle count high
    health_data[4] = 9; // Cycle count low
    health_data[5] = 4; // Health array for idk
    health_data[6] = 2; // Health array for idk
    health_data[7] = 0; // Health array for idk
    health_data[8] = 1; // Health array for idk
    health_data[9] = '\0'; // Null terminator, used so that we can call strlen for the length
    #ifdef DEBUG
        // Debug message, will contain each character in error_data as
        // a 2 character hex value, with spaces in between
        char debug_msg[HEALTH_DATA_LENGTH * 3 + 2];

        int i = 0;
        for(i = 0; i < HEALTH_DATA_LENGTH; ++i) {
            // Print every byte in error_data as a 3 character string, with 2 characters for a
            // hex value, and a space afterwards.
            // Have to print 4 characters, since snprintf automatically makes the last character
            // a null terminator. So, we have to overwrite the null terminator on all but the last
            // print. However, it does add the null terminator for us, so that's nice
            snprintf(&debug_msg[i*3], 4, "%02x ", health_data[i]);
        }

        UARTDebugSend("\r\nHealth Data:\r\n", 16);
	    UARTDebugSend(debug_msg, strlen(debug_msg));
    #endif // DEBUG //

	// Transmit data
	UARTOBCSend(health_data, strlen(health_data));
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
    // Finally, an escape character is sent to signal the start of data transmission
	if(uart_obc_msg_chars[0] == UART_OBC_ESCAPE &&
	   uart_obc_msg_chars[1] == UART_OBC_SOM && 
       uart_obc_msg_chars[2] == UART_OBC_ESCAPE) {
		// The command IDs are kind of stupid, but it's not like we're limited on 
		// transmission ability, and Calvin and Hayden (Rozsell) thought it was funny
		// Can't say I disagree - Nikhil
		if(uart_obc_msg_chars[3] == 'M' &&
		   uart_obc_msg_chars[4] == 'D') {
			// OBC wants the health and error data, so send it to them

            #ifdef DEBUG
                char msg[] = "Return health and error data\r\n";
			    UARTDebugSend(msg, strlen(msg));
            #endif /* DEBUG */

            // Send message beginning characters
			char start_msg[] = {UART_OBC_ESCAPE, UART_OBC_EOM, '\0'};
			UARTOBCSend(start_msg, strlen(start_msg));

			// Transmit Health and errors should each prepend a data packet with an escape character
            TransmitHealth();
            TransmitErrors();

            char term_msg[] = {UART_OBC_ESCAPE, UART_OBC_EOM, '\0'};
            UARTOBCSend(term_msg, strlen(term_msg));
		}
		else if(uart_obc_msg_chars[3] == 'D' &&
		        uart_obc_msg_chars[4] == 'M') {
			// OBC wants us to clear the health and error data

            // If we're debugging, send output to Debug UART
            #ifdef DEBUG
                char debug_msg[] = "Clear health data\r\n";
			    UARTDebugSend(debug_msg, strlen(debug_msg));

            #endif /* DEBUG */

            // Clear errors
            // Not sure how??
            // Get whether clearing errors was successful
            // Check if clearing was successful

            // If successful, send an ACK
            char msg[] = {UART_OBC_ESCAPE, UART_OBC_SOM, UART_OBC_ACK, UART_OBC_ESCAPE, UART_OBC_EOM, 0x00};
            
            // Else, send a NAK to tell OBC we couldn't clear the data for some reason
            // char msg[] = {UART_OBC_ESCAPE, UART_OBC_SOM, UART_OBC_NAK, UART_OBC_ESCAPE, UART_OBC_EOM, 0x00};


            UARTOBCSend(msg, strlen(msg));
            // If debug is enabled, also send the output to the debug prompt
            #ifdef DEBUG
                UARTDebugSend(msg, strlen(msg));
            #endif /* DEBUG */

		}
		else {
			// Tell OBC what they said makes no sense, but they were speaking our language
			// (i.e. error, but different than below)
			char bad_cmd_msg[] =  {UART_OBC_ESCAPE, UART_OBC_SOM, UART_OBC_ESCAPE, UART_OBC_ERROR_BAD_COMMAND, UART_OBC_ESCAPE, UART_OBC_EOM, 0x00};
			UARTOBCSend(bad_cmd_msg, strlen(bad_cmd_msg));
		}
	}
	else {
		// Tell OBC they aren't speaking our language
		// (i.e. error saying what they sent doesn't match the protocol)
        char bad_packet_msg[] =  {UART_OBC_ESCAPE, UART_OBC_SOM, UART_OBC_ESCAPE, UART_OBC_ERROR_BAD_PACKET, UART_OBC_ESCAPE, UART_OBC_EOM, 0x00};
		UARTOBCSend(bad_packet_msg, strlen(bad_packet_msg));
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

void MDERequestReset(void) {
	char msg[] = {UART_OBC_ESCAPE, UART_OBC_SOM, UART_OBC_RESET_MDE, UART_OBC_ESCAPE, UART_OBC_EOM, 0x00};
    UARTDebugSend(msg, strlen(msg));
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
