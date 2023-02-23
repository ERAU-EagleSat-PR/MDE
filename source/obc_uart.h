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
void FormatErrorDataPacket(void);
void FormatHealthDataPacket(void);
void TransmitErrors(void);
void TramsmitHealth(void);

// TODO - These functions are not yet implemented
//void UARTOBCRecvMsgHandler(void);
//void UARTOBCResponseHandler(void);

/*
*******************************************************************************
*                  MDE-OBC UART Protocol and Message Handling                 *
*******************************************************************************
*/

//These varibales will be used to structure the transmission of health and
// error data. The data will be sent in packets, each packet will have a packet
// type ID to denote what type of data is being sent. 

/* Start and End of Message Identifiers*/
#define UART_OBC_SOM    0x7F // Start of Message
#define UART_OBC_EOM    0xFF // End of Message
#define UART_OBC_ESCAPE 0x1F // Escape Character (splits packets)

/* Packet Type Identifiers */
#define UART_OBC_HEALTH_PACKET 0x01 // Health Packet Type
#define UART_OBC_ERROR_PACKET  0x10 // Error Packet Type

/* Max Incoming UART message size from OBC */
#define UART_OBC_MAX_MSG_SIZE  64   // 64 characters (32 bits each)

/* Flag to indicate an incoming message form the OBC (NOT USED) */
extern bool uart_obc_msg_flag;
extern bool* uart_obc_msg_flag_ptr;

// variable and pointer for incoming message
/*
extern uint8_t uart_obc_msg;
extern uint8_t* uart_obc_msg_ptr;
//*/


/*
*******************************************************************************
*                             UART Hardware Defines                           *
*******************************************************************************
*/

// The primary UART base to read and write to (OBC Communication)
#define ENABLE_UART_OBC

// Register base for OBC UART
#define UART_OBC        UART1_BASE
#define UART_OBC_BASE   UART1_BASE
#define UART_OBC_SYSCTL SYSCTL_PERIPH_UART1

// GPIO for OBC UART
#define UART_OBC_PORT_BASE  GPIO_PORTB_BASE
#define UART_OBC_SYSCTL     SYSCTL_PERIPH_GPIOB
#define UART_OBC_RX_PIN     GPIO_PIN_0
#define UART_OBC_TX_PIN     GPIO_PIN_1
#define UART_OBC_RX_PIN_CFG GPIO_PB0_U1RX
#define UART_OBC_TX_PIN_CFG GPIO_PB1_U1TX

#define UART_OBC_BAUD_RATE  115200


/*
*******************************************************************************
*                          Error Data Packet Defines                          *
*******************************************************************************
*/

// NOTE: In the next two sections, defining shifts and masks for the messages
// masks are applied before shifts!

// The value placed in the two bit header of each error message
// Valid values are 0 to 3
#define ERROR_DATA_HEADER_VALUE 3
// The shifts and masks for the error data, in case the error data formatting
// needs to be redefined. Adding data needs changes in each chip .c file.
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
// The value placed in the two bit header of each health message
// Valid values are 0 to 3
#define HEALTH_DATA_HEADER_VALUE 2
// The shifts and masks for the health data, in case the health data formatting
// needs to be redefined. Adding data needs changes in main .c file.
#define HEALTH_DATA_HEADER_SHIFT    56                     //while byte header
#define HEALTH_DATA_HEADER_MASK     0xF                    //1111h
#define HEALTH_DATA_CYCLE_SHIFT     48
#define HEALTH_DATA_CYCLE_MASK      0x3FFF // Only 14 bits allocated for the count
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

