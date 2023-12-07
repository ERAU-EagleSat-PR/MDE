/*
 * obc.h
 *
 * Contains code for communication via UART with the OBC.
 *
 */

#ifndef SOURCE_OBC_H_
#define SOURCE_OBC_H_

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/
// Handles an incoming message - stores characters in buffer and responds
// All logic occurs in the interrupt handler.
void UARTOBCIntHandler(void);
// Initializes the OBC UART
void UARTOBCEnable(void);
// Sends a series of uint8_t's to the OBC
void UARTOBCSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
// Sends all error data available
void TransmitErrors(void);
// Sends a health packet
void TramsmitHealth(void);

// Should be called in the main while loop to send a command to OBC to reset MDE
// (turn it off)
void MDERequestReset(void);

#ifdef DEBUG
// Functions used for debugging
void UARTOBCSetMsg(const uint8_t *pui8Buffer, uint32_t ui32Count);

#endif /* DEBUG */

/*
*******************************************************************************
*                  MDE-OBC UART Protocol and Message Handling                 *
*******************************************************************************
*/

// These variables will be used to structure the transmission of health and
// error data. The data will be sent in packets, each packet will have a packet
// type ID to denote what type of data is being sent. 

/* Start and End of Message Identifiers*/
#define UART_OBC_SOM    0x7F // Start of Message
#define UART_OBC_EOM    0xFF // End of Message
#define UART_OBC_ESCAPE 0x1F // Escape Character (splits packets)

/* MDE Response Characters */
#define UART_OBC_NAK 0x15 // NAK - sent if we have no data to report

/* Packet Type Identifiers */
#define UART_OBC_HEALTH_PACKET 0x01 // Health Packet Type
#define UART_OBC_ERROR_PACKET  0x10 // Error Packet Type

/* Max Incoming UART message size from OBC */
#define UART_OBC_MAX_MSG_SIZE  64   // 64 characters (8 bits each)

/*
*******************************************************************************
*                             UART Hardware Defines                           *
*******************************************************************************
*/

// The System Peripheral for the GPIO pins the 
// OBC UART uses 
#define UART_OBC_SYSCTL_GPIO SYSCTL_PERIPH_GPIOB

// The System Peripheral driver value for the
// OBC UART, UART 1
#define UART_OBC_SYSCTL_UART SYSCTL_PERIPH_UART1

// The GPIO Port Base for the OBC UART
#define UART_OBC_GPIO_PORT_BASE GPIO_PORTB_BASE

// The UART Port Base for the OBC UART
#define UART_OBC_UART_PORT_BASE UART1_BASE

// The set baud rate for the UART
#define UART_OBC_BAUD_RATE 115200

// The value to pass to IntEnable to enable Interrupts for the OBC UART
#define INT_UART_OBC INT_UART1

// Setting which GPIO pins in the base are used for the UART
// You need to check the documentation to see which pins are used
// For each UART on the microcontroller
#define UART_OBC_RX_PIN GPIO_PIN_0
#define UART_OBC_TX_PIN GPIO_PIN_1

// Values to pass to GPIOPinConfigure to configure the GPIO pins that the
// OBC UART uses to be UART pins
#define UART_OBC_RX_PIN_CFG GPIO_PB0_U1RX
#define UART_OBC_TX_PIN_CFG GPIO_PB1_U1TX

#define ERROR_DATA_LENGTH   10 // Length of the error packet in bytes, including the preceding escape character
#define HEALTH_DATA_LENGTH  9 // Length of the health packet in bytes, including the preceding escape character

#endif /* SOURCE_OBC_H_ */

