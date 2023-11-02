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
#include <stdlib.h>
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

// Custom Memory Drivers
#include "chipDrivers/FLASHfunc.h"
#include "chipDrivers/FRAMfunc.h"
#include "chipDrivers/MRAMfunc.h"
#include "chipDrivers/SRAMfunc.h"

// Additional Includes
#include "source/devtools.h"
#include "source/multiplexer.h"
#include "source/chips.h"
#include "source/mde.h"
#include "source/mde_timers.h"
#include "source/chip_health.h"
#include "source/bit_errors.h"
#include "source/obc_uart.h"


/*
 *******************************************************************************
 *                         Developer and Debug Functions                       *
 *******************************************************************************
 */

//-----------------------------------------------------------------------------
// The UART0 interrupt handler. Debugging UART
//-----------------------------------------------------------------------------
void
UARTDebugIntHandler(void)
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
void
UARTDebugEnable(void)
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
void
UARTDebugSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
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
void
printDebugMenu(void)
{
    // Prints the debug menu depending on current state

    // Variables for the menu
    char buf[60];
    uint8_t bufSize = 60;
    // Create local variable from global chip tracker to prevent overwrite
    uint8_t workingChip = current_chip % 16;
    uint8_t displayChip = workingChip + 1; //Chip Variable offset by 1 for better human understanding
    uint8_t workingBoard = selectedBoard;

    // Determine chip type
    char chipBuf[10];
    if(workingChip <= 3) {
        snprintf(chipBuf,10,"(FLASH)");
    } else if(workingChip <= 7){
        snprintf(chipBuf,10,"(FRAM)");
    } else if(workingChip <= 11){
        snprintf(chipBuf,10,"(MRAM)");
    } else if(workingChip <= 15){
        snprintf(chipBuf,10,"(SRAM)");
    } else {
        snprintf(chipBuf,10,"(error)");
    }




    // Begin menu
    switch (menuState)
    {
    case INIT:
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf, bufSize, "MDE Debug and Development Interface\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case MAIN:
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf, bufSize,  "Menu Selection:\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "M - Return to this menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "C - Clear screen.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "Q - Swap currently selected chip.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "\tCurrently Selected: %d",displayChip);
        snprintf(buf+strlen(buf),bufSize, chipBuf);
        snprintf(buf+strlen(buf),bufSize, "\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "F - Chip Functions.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "H - Chip Health Functions.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "B - Change currently selected board: %d\n\r",workingBoard);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "E - Error queue functions.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "A - Enter Auto/Flight Mode \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        // OBC Debug Options
        snprintf(buf, bufSize,  "O - OBC UART Commands\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        // Board Power functions
        snprintf(buf, bufSize,  "P - Board Power Control\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "X - Restart Program.\n\r");                       // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case CHIP_FUNCTIONS:
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf,bufSize, "Functions for currently selected chip: %d ", displayChip);
        snprintf(buf+strlen(buf),bufSize, chipBuf);
        snprintf(buf+strlen(buf),bufSize, "\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "M - Return to main menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "W - Write to chip.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "R - Read from chip.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        if(seedErrors == 1)
            snprintf(buf,bufSize, "E - Disable error seeding.\r\n");
        else
            snprintf(buf,bufSize, "E - Enable error seeding.\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "P - Prepare Status Register (MRAM/SRAM only)\n\r");
        UARTDebugSend((uint8_t*)buf, strlen(buf));
        snprintf(buf,bufSize, "S - Read Status Register \n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "T - Perform an entire MDE chip cycle.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "D - Swap Data. Current: %d\n\r",currentCycle);
        UARTDebugSend((uint8_t*)buf, strlen(buf));
        snprintf(buf, bufSize,"Q - Swap currently selected chip.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case CHIP_HEALTH:
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf,bufSize, "Chip Health and Data Functions\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "M - Return to main menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize,   "P - Print chip health array.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize,   "D - Print chip death array.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "H - Send Health Packet (TransmitHealth) \n\r");   // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize,  "E - Send Data Packet (TransmitErrors) \n\r");     // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case ERROR_QUEUE:
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf,bufSize, "Chip Error Queue Functions\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "M - Return to main menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "I - Insert a new error link to the queue.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "R - Delete the first link from the queue.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "D - Delete the entire queue.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "P - Print the entire error queue.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case CHIP_SELECT:
        UARTCharPut(UART_DEBUG, 0xC);
        if(chipSelectStep == 1)
        {
            snprintf(buf,bufSize, "Select a chip type:\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "F - Flash (1 - 4)\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "R - FRAM (5 - 8)\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "M - MRAM (9 - 12)\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "S - SRAM (13 - 16)\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        else if(chipSelectStep == 2)
        {
            snprintf(buf,bufSize, "Press a number 1-4 to choose the specific chip.\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "1 - First Chip\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "2 - Second Chip\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "3 - Third Chip\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "4 - Fourth Chip\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            snprintf(buf,bufSize, "B - Back to step 1.\r\n");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        break;

    case AUTO:
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf, bufSize, "AUTO MODE. Cycle time: %d Cycle count: %d\r\n",MEMORY_CYCLE_TIME, MEMORY_CYCLE_COUNT);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf, bufSize, "Hit M to exit.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
		
    case OBC_COMMANDS:
        snprintf(buf,bufSize, "G - Get Health Data\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "C - Clear Health Data\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "R - Request Power Off\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Q - Return to main menu\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case BOARD_POWER:
        snprintf(buf,bufSize, "1 - Turn on Board 1\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "! - Turn off Board 1\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "2 - Turn on Board 2\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "@ - Turn off Board 2\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Q - Return to main menu\r\n");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    default:
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf, bufSize, "State failure!");
    }

}

//-----------------------------------------------------------------------------
// Direct input to proper handler
//-----------------------------------------------------------------------------
void
processDebugInput(int32_t recv_char)
{

    switch (menuState)
    {
    case INIT:
        menuState = MAIN;
        printDebugMenu();
        break;
    case MAIN:
        processMainMenuInput(recv_char);
        break;
    case CHIP_SELECT:
        processChipSelectInput(recv_char);
        break;
    case CHIP_FUNCTIONS:
        processChipFunctionsInput(recv_char);
        break;
    case CHIP_HEALTH:
        processChipHealthInput(recv_char);
        break;
    case ERROR_QUEUE:
        processErrorInput(recv_char);
		break;
    case OBC_COMMANDS:
        processOBCCommandInput(recv_char);
        break;
    case BOARD_POWER:
        processBoardPowerInput(recv_char);
        break;
    case AUTO:
        switch (recv_char)
        {
        case 'm':
            MDETimerDisable();
            menuState = MAIN;
            break;
        }
        break;
    }

}

//-----------------------------------------------------------------------------
// Process input from the debug menu
//-----------------------------------------------------------------------------
void
processMainMenuInput(int32_t recv_char)
{
    IntMasterDisable();
    char buf[50];
    uint8_t bufSize = 50;

    switch (recv_char)
    {
    case 'm':   // Stay on Main Menu
        menuState = MAIN;
        printDebugMenu();
        break;
    case 'c':   // Clear the screen
        UARTCharPut(UART_DEBUG, 0xC);
        printDebugMenu();
        break;
    case 'f':   // Enter Chip Functions/Options
        menuState = CHIP_FUNCTIONS;
        printDebugMenu();
        break;
    case 'q':
        menuState = CHIP_SELECT;
        printDebugMenu();
        break;
    case 'e':
        menuState = ERROR_QUEUE;
        printDebugMenu();
        break;
    case 'a':   // Enter Auto Mode
        menuState = AUTO;
        MDETimerEnable(); // Enable MDE loop
        printDebugMenu();

        break;
    case 'h':   // Health Functions
        UARTCharPut(UART_DEBUG, 0xC);
        menuState = CHIP_HEALTH;
        printDebugMenu();
        break;
    case 'o':   // Switch to OBC coms menu
        UARTCharPut(UART_DEBUG, 0xC);
        menuState = OBC_COMMANDS;
        printDebugMenu();
        break;
    case 'p':   // Switch to board power menu
        UARTCharPut(UART_DEBUG, 0xC);
        menuState = BOARD_POWER;
        printDebugMenu();
        break;
    case 'x':
        UARTCharPut(UART_DEBUG, 0xC);
        printDebugMenu();
        snprintf(buf, bufSize,  "Function Not Implemented.\n\r");              // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    }
    IntMasterEnable();
}

//-----------------------------------------------------------------------------
// Process Chip Functions Menu
//-----------------------------------------------------------------------------
void
processChipFunctionsInput(int32_t recv_char)
{
    IntMasterDisable();
    char buf[50];
    uint8_t bufSize = 50;
    uint8_t workingChip = current_chip % 16;
    uint8_t displayChip = workingChip + 1;
    //uint8_t workingBoard = selectedBoard; // unused currently

    switch (recv_char)
    {
    case 'm':   // Return to Main Menu
        menuState = MAIN;
        printDebugMenu();
        break;

    case 'c':
        UARTCharPut(UART_DEBUG, 0xC);
        printDebugMenu();
        break;

    case 'w': // Write to currently selected chip
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf,bufSize, "Writing to chip number %d.\n\r",displayChip);
        UARTDebugSend((uint8_t*) buf, strlen(buf));

        // Parse correct function from chip number
        if(workingChip <= 3){ // Flash Range
            FlashSequenceTransmit(currentCycle, workingChip);

        } else if(workingChip <= 7){ // FRAM Range
            FRAMSequenceTransmit(currentCycle, workingChip);

        } else if(workingChip <= 11){ // MRAM Range
            MRAMSequenceTransmit(currentCycle, workingChip);

        } else if(workingChip <= 15){ // SRAM Range
            SRAMSequenceTransmit(currentCycle, workingChip);

        } else { // Failure State
            UARTCharPut(UART_DEBUG, 0xC);
            snprintf(buf,bufSize, "Chip Number Out Of Bounds: %d.\n\r",displayChip);
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        // Closing statements
        snprintf(buf,bufSize, "Write complete for chip number %d.\n\r",displayChip);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Press C to return to the chip functions menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case 'r':
        UARTCharPut(UART_DEBUG, 0xC);
        snprintf(buf,bufSize, "Reading from chip number %d.\n\r",displayChip);
        UARTDebugSend((uint8_t*) buf, strlen(buf));

        // Read from correct chip type
        if (workingChip <= 3){ // FLASH
            FlashSequenceRetrieve(currentCycle,workingChip);

        } else if(workingChip <=7) { // FRAM
            FRAMSequenceRetrieve(currentCycle,workingChip);

        } else if(workingChip <=11) { // MRAM
            MRAMSequenceRetrieve(currentCycle, workingChip);

        } else if(workingChip <= 15) { // SRAM Range
            SRAMSequenceRetrieve(currentCycle, workingChip);

        } else { // Failure State
            UARTCharPut(UART_DEBUG, 0xC);
            snprintf(buf,bufSize, "Chip Number Out Of Bounds.\n\r");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        snprintf(buf,bufSize, "Read complete for chip number %d.\n\r",displayChip);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Press C to return to the chip functions menu.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case 'e':
        if (seedErrors == 1)
            seedErrors = 0;
        else
            seedErrors = 1;
        UARTCharPut(UART_DEBUG, 0xC);
        printDebugMenu();
        break;

    case 'b':
        ErrorQueue_PrintLink(errorHead, current_error);
        break;
    case 'p':
        // Write SR from correct chip type
        if(workingChip <=7) { // FRAMFLASH
            snprintf(buf,bufSize, "Nothing to prepare for this chip type.\n\r");
            UARTDebugSend((uint8_t*) buf, strlen(buf));

        } else if(workingChip <=11) { // MRAM
            MRAMStatusPrepare(workingChip);

        } else { // Failure State
            snprintf(buf,bufSize, "Nothing to prepare for this chip type.\n\r");
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        break;

    case 's':
        // Read SR from correct chip type

        if (workingChip <= 3){ // FLASH
            FLASHID idFlash; // struct for ID parts
            idFlash = FlashStatusRead(workingChip);
          
            //Print struct to console
            sprintf(buf,"cypress id: %d \n\r",idFlash.cypID);
            UARTDebugSend((uint8_t*) buf,strlen(buf));
            sprintf(buf,"device id1: %d \n\r",idFlash.prodID1);
            UARTDebugSend((uint8_t*) buf,strlen(buf));
            sprintf(buf,"device id2: %d \n\r",idFlash.prodID2);
            UARTDebugSend((uint8_t*) buf,strlen(buf));
            sprintf(buf,"RDSR: %d \n\r",idFlash.RDSR); //Print to console
            UARTDebugSend((uint8_t*) buf,strlen(buf));

        } else if(workingChip <=7) { // FRAM
            FRAMID idFRAM; // struct for ID parts
            idFRAM = FRAMStatusRead(workingChip);
            // Print struct to console
            sprintf(buf,"fujitsi id: %d \n\r",idFRAM.fujID);
            UARTDebugSend((uint8_t*) buf,strlen(buf));
            sprintf(buf,"cont code: %d \n\r",idFRAM.contCode);
            UARTDebugSend((uint8_t*) buf,strlen(buf));
            sprintf(buf,"prod ID1: %d \n\r",idFRAM.prodID1);
            UARTDebugSend((uint8_t*) buf,strlen(buf));
            sprintf(buf,"prod ID2: %d \n\r",idFRAM.prodID2);
            UARTDebugSend((uint8_t*) buf,strlen(buf));

        } else if(workingChip <= 11) { // MRAM
            uint8_t mramSR = MRAMStatusRead(workingChip);
            snprintf(buf,bufSize, "SR output: "BYTE_TO_BINARY_PATTERN"\n\r", BYTE_TO_BINARY(mramSR));
            UARTDebugSend((uint8_t*) buf, strlen(buf));

        } else if(workingChip <= 15) { // SRAM Range
            uint8_t sramSR = SRAMStatusRead(workingChip);
            snprintf(buf,bufSize, "SR output: "BYTE_TO_BINARY_PATTERN"\n\r", BYTE_TO_BINARY(sramSR));
            UARTDebugSend((uint8_t*) buf, strlen(buf));

        } else { // Failure State
                UARTCharPut(UART_DEBUG, 0xC);
                snprintf(buf,bufSize, "Chip Number Out Of Bounds.\n\r");
                UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        break;
    case 't':
        snprintf(buf,bufSize, "Beginning cycle...\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));

        current_chip = 0;
        MDEProcessCycle();

        snprintf(buf,bufSize, "Cycle complete.\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case 'd':
        // Swap the current data cycle to the opposite
        if(currentCycle == 0)
        {
            currentCycle = 255;
        } else {
            currentCycle = 0;
        }
        snprintf(buf,bufSize, "Changed to %d's memory cycle.\n\r",currentCycle);              // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;

    case 'q':
        menuState = CHIP_SELECT;
        printDebugMenu();
        break;
    }

    IntMasterEnable();
}

//-----------------------------------------------------------------------------
// Process Chip Selection Menu
//-----------------------------------------------------------------------------
void
processChipSelectInput(int32_t recv_char)
{

    IntMasterDisable();
    // First, select chip type
    if(chipSelectStep == 1)
    {
        switch(recv_char)
        {
        // Set chip type base value
        case 'f': // Flash
            current_chip = 0;
            break;
        case 'r': // FRAM
            current_chip = 4;
            break;
        case 'm': // MRAM
            current_chip = 8;
            break;
        case 's': // SRAM
            current_chip = 12;
            break;
        }
        chipSelectStep = 2;
    }
    else if(chipSelectStep == 2)
    {
        switch(recv_char)
        {
        // Set chip type offset
        case '1':
            break;
        case '2':
            current_chip = current_chip + 1;
            break;
        case '3':
            current_chip = current_chip + 2;
            break;
        case '4':
            current_chip = current_chip + 3;
            break;
        case 'b':
            chipSelectStep = 1;
            printDebugMenu();
        }
        // Reset to base state
        chipSelectStep = 1;
        menuState = MAIN;
    }
    // Return to menu
    printDebugMenu();
    IntMasterEnable();
}

//-----------------------------------------------------------------------------
// Process Chip Health Functions Menu
//-----------------------------------------------------------------------------

void processChipHealthInput(int32_t recv_char)
{
    IntMasterDisable();
    char buf[20];
    uint8_t bufSize = 20;
    uint8_t i;

    switch (recv_char)
    {
    case 'm':   // Return to Main Menu
        menuState = MAIN;
        printDebugMenu();
        break;

    case 'p':
        for(i=0; i<MAX_CHIP_NUMBER; i++)
        {
            snprintf(buf, bufSize, "%4d" ,chip_health_array[i].HealthCount);
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        break;
    case 'd':
        for(i=0; i<MAX_CHIP_NUMBER; i++)
        {
            snprintf(buf, bufSize,  "%d" ,chip_death_array[i]);
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
        break;
    case 'h':
        snprintf(buf, bufSize,  "Function Not Implemented.\n\r");              // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    case 'e':
        snprintf(buf, bufSize,  "Function Not Implemented.\n\r");              // TODO
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        break;
    }

    snprintf(buf, bufSize,  "\r\n");
    UARTDebugSend((uint8_t*) buf, strlen(buf));
    IntMasterEnable();
}

//-----------------------------------------------------------------------------
// Process Error Queue Functions
//-----------------------------------------------------------------------------

void processErrorInput(int32_t recv_char)
{
    IntMasterDisable();

    switch (recv_char)
    {
    case 'm': // Return to main menu
        menuState = MAIN;
        printDebugMenu();
        break;
    case 'i': // Insert random new element
        // Use rand to get random numbers. We don't really care about the specifics of the randomness (i.e.
        // how uniform the distribution is) so it's fine to just call rand and take the modulus to limit the values
        // to acceptable ranges for the argument - i.e. uint8_t arguments will only get values between 0 and 255
        // The byte number/memory address is uint32_t, and rand() returns an int that is guaranteed to be positive,
        // so we don't have to worry about doing anything to the value from rand().
        // Note that RAND_MAX for the microcontroller is 32767, so the cell address is limited
        ErrorQueue_Insert(&errorHead, rand() % UINT8_MAX, rand(), rand() % UINT8_MAX, rand() % UINT8_MAX);
        current_error++;
        break;
    case 'r': // Delete first link, and print it.
        ErrorQueue_PrintLink(errorHead,1);
        ErrorQueue_Remove(&errorHead);
        current_error--;
        break;
    case 'd': // Delete the entire queue.
        ErrorQueue_Destroy(&errorHead);
        break;
    case 'p': // Print the entire queue.
        printDebugMenu();
        ErrorQueue_PrintLink(errorHead->next, current_error);
        break;
    }
    IntMasterEnable();
}
//-----------------------------------------------------------------------------
// Print the given error link
//-----------------------------------------------------------------------------
void ErrorQueue_PrintLink(MDE_Error_Data_t *ptr, uint8_t printCount)
{
    IntMasterDisable();
    char buf[40];
    uint8_t bufSize = 40;
    MDE_Error_Data_t *currentLink = ptr;
    uint8_t i;
    for(i = 0; i < printCount; i++)
    {
        snprintf(buf,bufSize, "MDE Error Packet: %p\n\r", currentLink);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Chip ID: %u\n\r", currentLink->chip_id);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Byte Address: 0x%X\n\r", currentLink->cell_address);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Written Sequence: %u\n\r", currentLink->written_sequence);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Retrieved Sequence: %u\n\r", currentLink->retrieved_sequence);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        snprintf(buf,bufSize, "Next pointer: %p\n\r", currentLink->next);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
        currentLink = currentLink->next;
    }
    IntMasterEnable();
}
//-----------------------------------------------------------------------------
// Process Board Power Menu
//-----------------------------------------------------------------------------
void processBoardPowerInput(int32_t recv_char)
{
    IntMasterDisable();

    switch (recv_char) {
        case '1':
            UARTCharPut(UART_DEBUG, 0xC);
            Board1PowerOn();
            printDebugMenu();
            UARTDebugSend("Turning on Board 1\r\n", 20);
            break;
        case '!':
            UARTCharPut(UART_DEBUG, 0xC);
            Board1PowerOff();
            printDebugMenu();
            UARTDebugSend("Turning off Board 1\r\n", 21);
            break;
        case '2':
            UARTCharPut(UART_DEBUG, 0xC);
            Board2PowerOn();
            printDebugMenu();
            UARTDebugSend("Turning on Board 2\r\n", 20);
            break;
        case '@':
            UARTCharPut(UART_DEBUG, 0xC);
            Board2PowerOff();
            printDebugMenu();
            UARTDebugSend("Turning off Board 2\r\n", 21);
            break;
        case 'q':
            UARTCharPut(UART_DEBUG, 0xC);
            menuState = MAIN;
            printDebugMenu();
            break;
        default:
            break;
    }

    // Return to menu
    IntMasterEnable();
}


//-----------------------------------------------------------------------------
// Process OBC Selection Menu
//-----------------------------------------------------------------------------
void processOBCCommandInput(int32_t recv_char)
{
    IntMasterDisable();
    
    uint8_t get_msg[] = {0x1F, 0x7F, 'M', 'D', 0x1F, 0xFF};
    uint8_t clr_msg[] = {0x1F, 0x7F, 'D', 'M', 0x1F, 0xFF};

    switch (recv_char) {
        case 'g':
            UARTCharPut(UART_DEBUG, 0xC);
            UARTOBCSetMsg(get_msg, 7);
            printDebugMenu();
            break;
        case 'c':
            UARTCharPut(UART_DEBUG, 0xC);
            UARTOBCSetMsg(clr_msg, 7);
            printDebugMenu();
            break;
        case 'r':
            UARTCharPut(UART_DEBUG, 0xC);
            MDERequestReset();
            printDebugMenu();
            UARTDebugSend("Request Power Off\r\n", 19);
            break;
        case 'q':
            UARTCharPut(UART_DEBUG, 0xC);
            menuState = MAIN;
            printDebugMenu();
            break;
        default:

            break;
    }

    // Return to menu
    IntMasterEnable();
}

