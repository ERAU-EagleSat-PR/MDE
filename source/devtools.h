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
void processChipFunctionsInput(int32_t recv_char);
void processChipSelectInput(int32_t recv_char);

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
                    CHIP_SELECT,
                    CHIP_FUNCTIONS};

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
extern enum MENU_STATES menuState;
extern enum BOARDS selectedBoardNumber;
extern enum MEM_TYPES selectedChipType;
extern enum CHIP_NUMBERS selectedChipNumber;
//*/

// Global variables for devtools
extern uint8_t selectedChip;   // Value 0-15
extern uint8_t selectedBoard;  // Value 0 or 1, will be changed to work as an offset when a second board is necessary in testing
extern uint8_t currentCycle;   // Value 0 or 1 for writing 0s or 1s
extern uint8_t chipSelectStep; // Used for chip type -> chip number step tracking

//-----------------------------------------------------------------------------
// Seed Errors
//-----------------------------------------------------------------------------

//#define SEEDERRORS

// Whether to seed errors in the chips, and where and what to seed.
// Inserting errors at addresses that are multiples of 256 are easiest to verify
// as any address that is a multiple of 256 will have the same value as the
// sequence start value. At these addresses, it's easy to find how many errors
// there should be by comparing the seeded error value and sequence start value.
// To enable or disable seeded errors, comment or un-comment the following lines.

//#define SEEDERRORS_ADDRESS  65536
//#define SEEDERRORS_VALUE    0xFF

// Byte pattern for printing in binary
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')


#endif /* DEBUG */

#endif /* SOURCE_DEVTOOLS_H_ */


