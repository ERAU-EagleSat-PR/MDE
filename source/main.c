/*
 * Main File for the EagleSat-2 Memory Degredation Experiment
 */

/*
*******************************************************************************
*                               Include Files                                 *
*******************************************************************************
*/

// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware Files
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

// Driver Files
#include <driverlib/debug.h>
#include <driverlib/fpu.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/rom.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>

// Custom Header Files
#include "source/mde_system.h"
#include "source/mde.h"

/*
*******************************************************************************
*                             Global Variables                                *
*******************************************************************************
*/

// Loop variable for blink
volatile uint32_t ui32Loop;

// Variables for the timer
uint32_t timer_current_cycle = 0;
bool timer_wakeup = false;

// The current clock speed
uint32_t SysClkSpeed = 16000000;

// State trackers for the menu
enum MENU_STATES menu_state = INIT;
uint32_t selectedBoardNumber = 0;
uint32_t selectedChipType = 0;
uint32_t selectedChipNumber = 0;

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/

// UART Functions
void EnablePrimaryUART(void);
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UART0IntHandler(void);
void EnableDebugUART(void);
void UARTSendDebug(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UARTSendDebug(const uint8_t *pui8Buffer, uint32_t ui32Count);
void printMenu(void);
void processDebugInput(int32_t recv_char);
void processMainMenuInput(int32_t recv_char);
void processCSBoardMenuInput(int32_t recv_char);
void processCSTypeMenuInput(int32_t recv_char);
void processCSNumMenuInput(int32_t recv_char);

// LED Functions
void EnableLED(void);
void BlinkRedLED(void);
void BlinkBlueLED(void);
void BlinkGreenLED(void);


/*
*******************************************************************************
*                               UART Functions                                *
*******************************************************************************
*/


//-----------------------------------------------------------------------------
// Enable and configure UART1 for science
//-----------------------------------------------------------------------------
/*
void EnablePrimaryUART(void)
{

}
*/

//-----------------------------------------------------------------------------
// Send a string to the Primary UART.
//-----------------------------------------------------------------------------
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(UART_PRIMARY, *pui8Buffer++);
    }
}


#ifdef ENABLE_UART_DEBUG //////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// The UART0 interrupt handler. Debugging UART
//-----------------------------------------------------------------------------
void UART0IntHandler(void)
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
void EnableDebugUART(void)
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
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                       (UART_CONFIG_WLEN_8 |
                        UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE));

    // Map the Interrupt handler for UART 0
    UARTIntRegister( UART_DEBUG , UART0IntHandler );

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
void printMenu(void)
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
            printMenu();
            break;
        case MAIN:
            processMainMenuInput(recv_char);
            break;
        case CHIP_SELECT_BOARD:
            processCSBoardMenuInput(recv_char);
            break;
        case CHIP_SELECT_TYPE:
            processCSTypeMenuInput(recv_char);
            break;
        case CHIP_SELECT_NUM:
            processCSNumMenuInput(recv_char);
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
            printMenu();
            break;
        case 'c':   // Clear the screen
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            break;
        case 'q':   // Manually select/enable chip
            menu_state = CHIP_SELECT_BOARD;
            printMenu();
            break;
        case 'a':   // Enter Auto Mode
            //menu_state = AUTO;
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        case 'h':   // Health Packet
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        case 'e':   // Data/ Error Packet
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        case 'x':
            UARTCharPut(UART_DEBUG, 0xC);
            printMenu();
            sprintf(buf, "Function Not Implemented.\n\r");              // TODO
            UARTDebugSend((uint8_t*) buf, strlen(buf));
            break;
        }

}

//-----------------------------------------------------------------------------
// Process Board Selection Menu
//-----------------------------------------------------------------------------
void processCSBoardMenuInput(int32_t recv_char)
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

    printMenu();

    IntMasterEnable();
}

//-----------------------------------------------------------------------------
// Process Memory Type Selection Menu
//-----------------------------------------------------------------------------
void processCSTypeMenuInput(int32_t recv_char)
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
    printMenu();

    IntMasterEnable();

}

//-----------------------------------------------------------------------------
// Process Chip Selection Menu
//-----------------------------------------------------------------------------
void processCSNumMenuInput(int32_t recv_char)
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

    printMenu();
    // TODO delete later
    char buf[200];
    sprintf(buf, "Chip Number = %d \n\r", selectedChipNumber);
            UARTDebugSend((uint8_t*) buf, strlen(buf));

    //Enable the Selected Chip
    setCSOutput(selectedChipNumber);

    IntMasterEnable();
}

#endif ////////////////////////////////////////////////////////////////////////

/*
*******************************************************************************
*                               Chip Select Pins                              *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// Enables GPIO for Board 1 chip select
//-----------------------------------------------------------------------------
void EnableBoard1ChipSelectPins(void)
{
    IntMasterDisable();

    // Enable GPIO
    SysCtlPeripheralEnable(BOARD1_CS_PORT_SYSCTL);

    // wait to be ready
    while(!SysCtlPeripheralReady(BOARD1_CS_PORT_SYSCTL))
    {
    }

    //Set pins as output
    GPIOPinTypeGPIOOutput(BOARD1_CS_PORT_BASE,
                          CS1_PIN_0 | CS1_PIN_1 | CS1_PIN_2 | CS1_PIN_3 );

    IntMasterEnable();
}

//-----------------------------------------------------------------------------
// Enables GPIO for Board 2 chip select
//-----------------------------------------------------------------------------
void EnableBoard2ChipSelectPins(void)
{
    IntMasterDisable();

    // Enable GPIO for Board 2 chip select
    SysCtlPeripheralEnable(BOARD2_CS_PORT_SYSCTL);

    // Wait to be ready
    while(!SysCtlPeripheralReady(BOARD2_CS_PORT_SYSCTL))
    {
    }

    // Set pins as output
    GPIOPinTypeGPIOOutput(BOARD2_CS_PORT_BASE,
                          CS2_PIN_0 | CS2_PIN_1 | CS2_PIN_2 | CS2_PIN_3 );

    IntMasterEnable();
}

/*
*******************************************************************************
*                                LED FUNCTIONS                                *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// Enables the RGB LED
//-----------------------------------------------------------------------------
void EnableLED(void)
{

  // Enable LED GPIO PIN
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  // Check for peripheral to be Enabled
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
  {
  }

  // Enable the GPIO pin for the RED LED (PF1).  Set the direction as output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

  // Enable the GPIO pin for the GREEN LED (PF3).  Set the direction as output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

  // Enable the GPIO pin for the BLUE LED (PF2).  Set the direction as output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

}

//-----------------------------------------------------------------------------
// Blinks the Red LED
//-----------------------------------------------------------------------------
void BlinkRedLED(void)
{

    // Turn on RED LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

    // Delay for a bit.
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }

    // Turn off RED LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }
}

//-----------------------------------------------------------------------------
// Blinks the Blue LED, Signifying the MDE is IDLE / WAITING
//-----------------------------------------------------------------------------
void BlinkBlueLED(void)
{

    // Turn on BLUE LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

    // Delay for a bit.
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }

    // Turn off BLUE LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }
}

//-----------------------------------------------------------------------------
// Blinks the Green LED
//-----------------------------------------------------------------------------
void BlinkGreenLED(void)
{

    // Turn on GREEN LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

    // Delay for a bit.
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }

    // Turn off GREEN LED.
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

    // Delay for a bit
    for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//                                                                            *
//                                      MAIN                                  *
//                                                                            *
//*****************************************************************************
int main(void)
{

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    FPUEnable();
    FPULazyStackingEnable();

    // Set the clock speed.
    SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320,
                       SYS_CLK_SPEED);

    // Enable processor interrupts.
    IntMasterEnable();

    //*****************************
    // Peripheral Enablers
    //*****************************

    EnableLED();

    EnableBoard1ChipSelectPins();

    EnableBoard2ChipSelectPins();


#ifdef DEBUG

    // UART Enable and Configuration
    EnableDebugUART();

    // Initialize Debug Menu
    menu_state = MAIN;
    printMenu();

#endif


    while (1)
    {
        // Idle "heart beat"
        BlinkBlueLED();
        //BlinkRedLED();
        //BlinkGreenLED();


        /*
        uint8_t code;
        int chipNum;

        for (chipNum = 0; chipNum <= 15; chipNum++)
        {
            //setCSOutput(chipNum);

            code = RetreiveCSCode(chipNum);

            GPIOPinWrite(BOARD1_CS_PORT_BASE,
                         CS1_PIN_0 | CS1_PIN_1 | CS1_PIN_2 | CS1_PIN_3 ,
                         code);
            GPIOPinWrite(BOARD2_CS_PORT_BASE,
                         CS2_PIN_0 | CS2_PIN_1 | CS2_PIN_2 | CS2_PIN_3 ,
                         code);

        }
        */

    }

    return 0;
}

//*****************************************************************************
//                                                                            *
//                                END MAIN                                    *
//                                                                            *
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
