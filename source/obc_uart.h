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
void UARTOBCIntHandler(void);
void UARTOBCEnable(void);
void UARTOBCSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
bool UARTOBCIsDataReady(void);
void FormatErrorDataPacket(void);
void FormatHealthDataPacket(void);
void TransmitErrors(void);
void TramsmitHealth(void);
void UARTOBCRecvMsgHandler(void);

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
#define UART_OBC_ACK 0x14 // ACK - sent after successful execution of a command that doesn't send anything back
#define UART_OBC_NAK 0x15 // NAK - sent after a command did not complete successfully for some reason
#define UART_OBC_RESET_MDE 'R'
#define UART_OBC_ERROR_BAD_PACKET 'P'
#define UART_OBC_ERROR_BAD_COMMAND 'C'

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

// Flag used to enable the OBC UART
#define ENABLE_UART_OBC

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
/*
*******************************************************************************
*                          Error Data Packet Defines                          *
*******************************************************************************
*/

// NOTE: In the next two sections, defining shifts and masks for the messages
// masks are applied before shifts!

// The shifts and masks for the error data, in case the error data formatting
// needs to be redefined. Adding data needs changes in each chip .c file.
#define ERROR_DATA_LENGTH           10 // Length of the packet in bytes, including the preceding escape character
#define ERROR_DATA_HEADER_SHIFT     38
#define ERROR_DATA_HEADER_MASK      0x3
#define ERROR_DATA_CHIP_SHIFT       32
#define ERROR_DATA_CHIP_MASK        0x3F
#define ERROR_DATA_BYTE_SHIFT       6
#define ERROR_DATA_BYTE_MASK        0x3FFFFF
#define ERROR_DATA_DIRECTION_SHIFT  3
#define ERROR_DATA_DIRECTION_MASK   0x1
#define ERROR_DATA_BIT_SHIFT        0
#define ERROR_DATA_BIT_MASK         0x7

/*
*******************************************************************************
*                               Health Data Packets                           *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// TransmitHealth() Health & Responsiveness Variables //TODO
//-----------------------------------------------------------------------------
// The shifts and masks for the health data, in case the health data formatting
// needs to be redefined. Adding data needs changes in main .c file.
#define HEALTH_DATA_LENGTH          9 // Length of the packet in bytes, including the preceding escape character
#define HEALTH_DATA_HEADER_SHIFT    56                     //while byte header
#define HEALTH_DATA_HEADER_MASK     0xF                    //1111h
#define HEALTH_DATA_CYCLE_SHIFT     48
#define HEALTH_DATA_CYCLE_MASK      0xFFFF
#define HEALTH_DATA_START_SHIFT     4
#define HEALTH_DATA_START_MASK      0xF
#define HEALTH_DATA_OFFSET_SHIFT    0
#define HEALTH_DATA_OFFSET_MASK     0xF
#define HEALTH_DATA_RESPONSIVENESS_SHIFT 8
#define HEALTH_DATA_RESPONSIVENESS_MASK  0xFFFFFFFFFF

// The unresponsiveness buffer
// Chips can be marked as unresponsive if they do not transmit any data
// Eventually this could be used to determine if a power cycle should happen
// This buffer is a single 64 bit value and each bit is written to indicate
// responsiveness. For example, bit 4 corresponds to chip 4.
extern uint64_t chip_unresponsive;

// The number of bytes to check before declaring a chip unresponsive
#define BLANK_BYTE_UNRESPONSIVE_COUNT 8

//health Packet fromatter
    /* Funtion prototype here */

//health Packet Sender
    /* Funtion prototype here */

#endif /* SOURCE_OBC_H_ */

