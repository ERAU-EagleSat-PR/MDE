/*
 * devtools.h
 *
 * The devtools.h/c files contain code related to tools used for development,
 * debugging and testing. This includes the debugger virtual com debug/dev
 * interface and seeding errors for testing
 *
 */

#ifndef SOURCE_DEVTOOLS_H_
#define SOURCE_DEVTOOLS_H_

// Whether we are running in debug mode or not.
// In debug mode, all output is printed in a human readable format, while in
// non-debug (flight) mode, all output is sent in the agreed packet format.
// To enable and disable debug mode, comment or un-comment the following line.
#define DEBUG
#ifdef DEBUG

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/

void UARTDebugIntHandler(void);
void UARTDebugEnable(void);
void UARTDebugSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
void printDebugMenu(void);
void processDebugInput(int32_t recv_char);
void processMainMenuInput(int32_t recv_char);
void processChipSelectBoardMenuInput(int32_t recv_char);
void processChipSelectMemTypeMenuInput(int32_t recv_char);
void processChipSelectChipNumMenuInput(int32_t recv_char);
char* getMemTypeString(void);


/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/

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

//-----------------------------------------------------------------------------
// State trackers for Debug menu
//-----------------------------------------------------------------------------
enum MENU_STATES {  INIT,
                    MAIN,
                    AUTO,
                    CHIP_SELECT_BOARD,
                    CHIP_SELECT_TYPE,
                    CHIP_SELECT_NUM};

enum BOARDS      {  NO_BOARD,
                    BOARD1,
                    BOARD2};

enum MEM_TYPES   {  NO_MEM_TYPE,
                    FLASH,
                    FRAM,
                    MRAM,
                    SRAM};

enum CHIP_NUMBERS{  NO_CHIP,
                    FLASH1, FLASH2, FLASH3, FLASH4,
                    FRAM1, FRAM2, FRAM3, FRAM4,
                    MRAM1, MRAM2, MRAM3, MRAM4,
                    SRAM1, SRAM2, SRAM3, SRAM4};

//Track the current menu selection
//*
extern enum MENU_STATES menu_state;
extern enum BOARDS selectedBoardNumber;
extern enum MEM_TYPES selectedChipType;
extern enum CHIP_NUMBERS selectedChipNumber;
//*/


//-----------------------------------------------------------------------------
// Seed Errors
//-----------------------------------------------------------------------------

#define SEEDERRORS

// Whether to seed errors in the chips, and where and what to seed.
// Inserting errors at addresses that are multiples of 256 are easiest to verify
// as any address that is a multiple of 256 will have the same value as the
// sequence start value. At these addresses, it's easy to find how many errors
// there should be by comparing the seeded error value and sequence start value.
// To enable or disable seeded errors, comment or un-comment the following lines.

#define SEEDERRORS_ADDRESS  65536
#define SEEDERRORS_VALUE    0xFF

#endif /* DEBUG */

#endif /* SOURCE_DEVTOOLS_H_ */


