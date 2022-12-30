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
void TransmitErrors(void);
void TramsmitHealth(void);

/*
*******************************************************************************
*                              Hardware Constants                             *
*******************************************************************************
*/


//-----------------------------------------------------------------------------
// UART Variables
//-----------------------------------------------------------------------------

// The primary UART base to read and write to (OBC Communication)
//#define ENABLE_UART_PRIMARY
#define UART_OBC        UART1_BASE
#define UART_OBC_BASE   UART1_BASE
#define UART_OBC_SYSCTL SYSCTL_PERIPH_UART1

#define BAUD_RATE_OBC   115200

// TODO
#define UART_OBC_PORT_BASE
#define UART_OBC_RX_PIN
#define UART_OBC_TX_PIN

// GPIO for OBC UART


//-----------------------------------------------------------------------------
// TransmitErrors() Error Buffer Variables //TODO
//-----------------------------------------------------------------------------

// The maximum amount of errors that can be stored
// If this is exceeded then the system loops back around, though the looping is
// sort of broken as when transmitting the system will forget about the previous
// set of errors.
#define ERROR_BUFFER_MAX_SIZE 1000
// The buffer variables
extern uint64_t error_buffer[ERROR_BUFFER_MAX_SIZE];
extern uint32_t current_error;
extern uint32_t old_current_error;

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


//-----------------------------------------------------------------------------
// TransmitHealth() Health & Responsiveness Variables //TODO
//-----------------------------------------------------------------------------
// The value placed in the two bit header of each health message
// Valid values are 0 to 3
#define HEALTH_DATA_HEADER_VALUE 2
// The shifts and masks for the health data, in case the health data formatting
// needs to be redefined. Adding data needs changes in main .c file.
#define HEALTH_DATA_HEADER_SHIFT    56                     //while byte header
#define HEALTH_DATA_HEADER_MASK     0xF                     // 11111111h
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

#endif /* SOURCE_OBC_H_ */

