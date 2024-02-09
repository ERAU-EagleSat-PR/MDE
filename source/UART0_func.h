/*
 *  UART0_func.h
 *
 *  The various timers for the MDE chips
 *
 *  Created on: Feb 9, 2024
 *      Author: Tyler
 */

#ifndef SOURCE_UART0_FUNC_H_
#define SOURCE_UART0_FUNC_H_

//UART communication
void UARTDebugIntHandler(void);
void UARTDebugEnable(void);
void UARTDebugSend(const uint8_t *pui8Buffer, uint32_t ui32Count);


//-----------------------------------------------------------------------------
// UART for Debug
//-----------------------------------------------------------------------------

// The UART to Use debug terminal through
#define ENABLE_UART_DEBUG

#define UART_DEBUG          UART0_BASE
#define UART_DEBUG_BASE     UART0_BASE
#define UART_DEBUG_SYSCTL   SYSCTL_PERIPH_UART0

// The baud rate to use for either UART connection
#define BAUD_RATE_DEBUG     115200

#endif
