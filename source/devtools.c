/*
 * devtools.c
 *
 *  Created on: Dec 29, 2022
 *      Author: Calvin
 */


/*
*******************************************************************************
*                                   Includes                                  *
*******************************************************************************
*/

// Standard C files
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

#include "source/devtools.h"
#include "source/multiplexer.h"

/*
*******************************************************************************
*                         Developer and Debug Functions                       *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// The UART0 interrupt handler. Debugging UART
//-----------------------------------------------------------------------------
void UARTDebugIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrupt status.
    //
    ui32Status = UARTIntStatus(UART_DEBUG, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART_DEBUG, ui32Status);

    //
    // Loop while there are unsigned characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART_DEBUG))
    {
        int32_t local_char;
        local_char = UARTCharGet(UART_DEBUG);
        if(local_char != -1)
        {
            processDebugInput(local_char);
        }
    }

}

//-----------------------------------------------------------------------------
// Enable and configure UART0 for debugging
//-----------------------------------------------------------------------------
void UARTDebugEnable(void)
{
    // Enable Debug UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Wait while un-ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    // Enable GPIO A for Virtual Com Port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Wait while un-ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BAUD_RATE_DEBUG,
                       (UART_CONFIG_WLEN_8 |
                        UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE));

    // Map the Interrupt handler for UART 0
    UARTIntRegister( UART_DEBUG , UARTDebugIntHandler );

    // Enable interrupts for UART 0
    UARTIntEnable( UART_DEBUG, UART_INT_RX | UART_INT_RT);
}


//-----------------------------------------------------------------------------
// Send a string to the Debug UART.
//-----------------------------------------------------------------------------
void UARTDebugSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(UART_DEBUG, *pui8Buffer++);
    }
}


//-----------------------------------------------------------------------------
// Print the debug/ development menu to the console
//-----------------------------------------------------------------------------
void printDebugMenu(void)
{

    char buf[200];

    switch (menu_state)
    {
    case INIT:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf,"MDE Debug and Development Interface\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case MAIN:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf, "Menu Selection:\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "M - Return to this menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "C - Clear screen.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "Q - Chip Functions.\n\r");                        // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "H - Send Health Packet (TransmitHealth) \n\r");   // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "E - Send Data Packet (TransmitErrors) \n\r");     // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "X - Restart Program.\n\r");                       // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case AUTO:
        sprintf(buf,"AUTO MODE. Hit M to exit.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case CHIP_SELECT_BOARD:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf, "Chip Select Menu (Board):\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "1 - Board 1\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "2 - Board 2\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "X - Return to Main Menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case CHIP_SELECT_TYPE:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf, "Chip Select Menu (TYPE):\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "Q - Flash \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "F - FRAM \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "M - MRAM \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "S - SRAM \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "Z - Previous Menu (Board) \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "X - Return to Main Menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case CHIP_SELECT_NUM:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf, "Chip Select Menu (Chip Number):\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "Selected Chip Type : %d \n\r", selectedChipType);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "1 - Chip 1 \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "2 - Chip 2 \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "3 - Chip 3 \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "4 - Chip 4 \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "Z - Previous Menu (Type) \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        sprintf(buf, "X - Return to Main Menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    default:
        UARTCharPut(UART_DEBUG, 0xC);
        sprintf(buf,"State failure!");
    }

}

//-----------------------------------------------------------------------------
// Direct input to proper handler
//-----------------------------------------------------------------------------
void processDebugInput(int32_t recv_char)
{

    switch (menu_state)
    {
        case INIT:
            menu_state = MAIN;
            printDebugMenu();
            break;
        case MAIN:
            processMainMenuInput(recv_char);
            break;
        case CHIP_SELECT_BOARD:
            processChipSelectBoardMenuInput(recv_char);
            break;
        case CHIP_SELECT_TYPE:
            processChipSelectMemTypeMenuInput(recv_char);
            break;
        case CHIP_SELECT_NUM:
            processChipSelectChipNumMenuInput(recv_char);
            break;
        case AUTO:
            switch (recv_char)
            {
                case 'm':
                    menu_state = MAIN;
                    break;
            }
        break;
    }

}

//-----------------------------------------------------------------------------
// Process input from the debug menu
//-----------------------------------------------------------------------------
void processMainMenuInput(int32_t recv_char)
{
    char buf[200];

    switch (recv_char)
    {
        case 'm':   // Stay on Main Menu
            menu_state = MAIN;
            printDebugMenu();
            break;
        case 'c':   // Clear the screen
            UARTCharPut(UART_DEBUG, 0xC);
            printDebugMenu();
            break;
        case 'q':   // Manually select/enable chip
            menu_state = CHIP_SELECT_BOARD;
            printDebugMenu();
            break;
        case 'a':   // Enter Auto Mode
            //menu_state = AUTO;
            UARTCharPut(UART_DEBUG, 0xC);
            printDebugMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        case 'h':   // Health Packet
            UARTCharPut(UART_DEBUG, 0xC);
            printDebugMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        case 'e':   // Data/ Error Packet
            UARTCharPut(UART_DEBUG, 0xC);
            printDebugMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        case 'x':
            UARTCharPut(UART_DEBUG, 0xC);
            printDebugMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        }

}

//-----------------------------------------------------------------------------
// Process Board Selection Menu
//-----------------------------------------------------------------------------
void processChipSelectBoardMenuInput(int32_t recv_char)
{

    IntMasterDisable();

    switch (recv_char)
    {
        case '1':   // Select Board 1
            selectedBoardNumber = BOARD1;
            menu_state = CHIP_SELECT_TYPE;;
            break;
        case '2':   // Select Board 2
            selectedBoardNumber = BOARD2;
            menu_state = CHIP_SELECT_TYPE;
            break;
        case 'x':   // Return to Main Menu
            selectedBoardNumber = NO_BOARD;
            selectedChipType = NO_MEM_TYPE;
            selectedChipNumber = NO_CHIP;
            menu_state = MAIN;
            break;
    }

    printDebugMenu();

    IntMasterEnable();
}

//-----------------------------------------------------------------------------
// Process Memory Type Selection Menu
//-----------------------------------------------------------------------------
void processChipSelectMemTypeMenuInput(int32_t recv_char)
{
    IntMasterDisable();

    switch (recv_char)
    {
        case 'q': // Flash Memory
            selectedChipType = FLASH;
            menu_state = CHIP_SELECT_NUM;
            break;
        case 'f': // FRAM
            selectedChipType = FRAM;
            menu_state = CHIP_SELECT_NUM;
            break;
        case 'm': // MRAM
            selectedChipType = MRAM;
            menu_state = CHIP_SELECT_NUM;
            break;
        case 's': // SRAM
            selectedChipType = SRAM;
            menu_state = CHIP_SELECT_NUM;
            break;
        case 'z': // Return to board selection
            selectedBoardNumber = NO_BOARD;
            selectedChipType = NO_MEM_TYPE;
            selectedChipNumber = NO_CHIP;
            menu_state = CHIP_SELECT_BOARD;
            break;
        case 'x': // Return to Main Menu
            selectedBoardNumber = NO_BOARD;
            selectedChipType = NO_MEM_TYPE;
            selectedChipNumber = NO_CHIP;
            menu_state = MAIN;
            break;
    }

    // Re-print menu with updated state
    printDebugMenu();

    IntMasterEnable();

}

//-----------------------------------------------------------------------------
// Process Chip Selection Menu
//-----------------------------------------------------------------------------
void processChipSelectChipNumMenuInput(int32_t recv_char)
{
    IntMasterDisable();

    switch (recv_char)
    {
        case '1':   // chip 1
            selectedChipNumber = selectedBoardNumber * selectedChipType;
            break;
        case '2':   // chip 2
            selectedChipNumber = 2 * selectedBoardNumber * selectedChipType;
            break;
        case '3':
            selectedChipNumber = 3 * selectedBoardNumber * selectedChipType;
            break;
        case '4':
            selectedChipNumber = 4 * selectedBoardNumber * selectedChipType;
            break;
        case 'z': // Return to type selection
            selectedChipType = NO_MEM_TYPE;
            selectedChipNumber = NO_CHIP;
            menu_state = CHIP_SELECT_TYPE;
            break;
        case 'x': // Return to Main Menu
            selectedBoardNumber = NO_BOARD;
            selectedChipType = NO_MEM_TYPE;
            selectedChipNumber = NO_CHIP;
            menu_state = MAIN;
            break;
    }

    printDebugMenu();

    //Enable the Selected Chip if chosen
    if (selectedChipNumber != NO_CHIP)
    {
        SetChipSelect(selectedChipNumber - 1);
    }

    IntMasterEnable();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
char getMemTypeString(uint16_t MemTypeEnum);
{
    switch (memTypeEnum)
    {
        case NO_MEM_TYPE:
            break;
        case NO_MEM_TYPE:
            break;
        case NO_MEM_TYPE:
            break;
        case NO_MEM_TYPE:
            break;
        case NO_MEM_TYPE:
            break;

    }
}



