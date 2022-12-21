/*
 * obc_integration.h
 *
 *  Created on: Dec 17, 2022
 *      Author: Calvin
 */

#ifndef SOURCE_OBC_INTEGRATION_H_
#define SOURCE_OBC_INTEGRATION_H_

//*****************************************
// UART Hardware Variables
//*****************************************

// The primary UART base to read and write to (OBC Communication)
#define UART_PRIMARY UART1_BASE
#define ENABLE_UART1 TRUE

// The UART to Use debug terminal through
#define UART_DEBUG UART0_BASE
#define ENABLE_UART0 TRUE

// The baud rate to use for either UART connection
#define BAUD_RATE 115200

#endif /* SOURCE_OBC_INTEGRATION_H_ */
