//*****************************************************************************
//
// The MDE Mux prototype.
// Works 100% of the time, 50% of the time
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "MDE_Mux_Prototype.h"
#include "Access_Tools.h"
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

//*****************************************************************************
//
// Initializing some global variables
//
//*****************************************************************************

// Variables for the timer
uint32_t timer_current_cycle = 0;
bool timer_wakeup = false;

// The current clock speed
uint32_t SysClkSpeed = 0;

// State trackers for the menu
enum MENU_STATES menu_state = INIT;
uint32_t selected_chip_number = 0;

//*****************************************************************************
//
// Function Definitions
//
//*****************************************************************************
//
// Enable chip select port/pins
//
//*****************************************************************************

void
EnableChipSelects(){

  // Chip Select Port
  SysCtlPeripheralEnable(CS_SYSCTL_PORT);

  //
  // Wait for peripheral to be enabled
  //
  while(!SysCtlPeripheralReady(CS0_SYSCTL_PORT) && !SysCtlPeripheralReady(CS1_SYSCTL_PORT))
  {
  }
    //
    // Configure the pins
    //
    // Board 1
    GPIOPinTypeGPIOOutput(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_0);
    GPIOPinTypeGPIOOutput(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_1);
    GPIOPinTypeGPIOOutput(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_2);
    GPIOPinTypeGPIOOutput(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_3);

    //Board 2
    GPIOPinTypeGPIOOutput(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_0);
    GPIOPinTypeGPIOOutput(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_1);
    GPIOPinTypeGPIOOutput(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_2);
    GPIOPinTypeGPIOOutput(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_3);

    //
    // Set chip select lines low for now
    //
    // Board 1
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_0, 0x0);
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_1, 0x0);
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_2, 0x0);
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_3, 0x0);

    //Board 2
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_0, 0x0);
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_1, 0x0);
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_2, 0x0);
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_3, 0x0);
  }
}
//*****************************************************************************
//
// Select the proper chip select lines, board 1 and 2
//
//*****************************************************************************

// Board 1
void
ChipSelect0(uint8_t chipNumber)
{
    // Retrieve the pin assignment for the given chip
    uint8_t chipPort = CHIP_SELECT_MUX_PINS(chipNumber);

    // Write chip select to pins
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_0, (chipPort & 0x1));
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_1, ((chipPort >> 1) & 0x1));
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_2, ((chipPort >> 2) & 0x1));
    GPIOPinWrite(CS0_SYSCTL_PORT, CS0_SYSCTL_PIN_3, ((chipPort >> 3) & 0x1));
}
//Board 2
void
ChipSelect1(uint8_t chipNumber)
{
    // Retrieve the pin assignment for the given chip
    uint8_t chipPort = CHIP_SELECT_MUX_PINS(chipNumber);

    // Write chip select to pins
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_0, (chipPort & 0x1));
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_1, ((chipPort >> 1) & 0x1));
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_2, ((chipPort >> 2) & 0x1));
    GPIOPinWrite(CS1_SYSCTL_PORT, CS1_SYSCTL_PIN_3, ((chipPort >> 3) & 0x1));
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
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
//*****************************************************************************
//
// The UART1 interrupt handler.
//
//*****************************************************************************
void
UART1IntHandler(void)
{
  uint32_t ui32Status;

  //
  // Get the interrupt status.
  //
  ui32Status = UARTIntStatus(UART1_BASE, true);

  //
  // Clear the asserted interrupts.
  //
  UARTIntClear(UART1_BASE, ui32Status);

  //
  // Loop while there are unsigned characters in the receive FIFO.
  //
  while(UARTCharsAvail(UART1_BASE))
  {

    int32_t local_char;
    local_char = UARTCharGet(UART1_BASE);
    if(local_char != -1)
    {
        process_menu(local_char);
    }
    print_menu();
  }
}
//*****************************************************************************
//
// Print the current menu configuration.
//
//*****************************************************************************
void
print_menu(void)
{

  char buf[200];

  switch (menu_state)
  {
  case INIT:
    UARTCharPut(UART_PRIMARY, 0xC);
    sprintf(buf,"MDE Mux Prototype Interface\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    break;
  case MAIN:
    UARTCharPut(UART_PRIMARY, 0xC);
    sprintf(buf,"Menu Selection:\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"M - Return to this menu.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"A - Enter Auto mode.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"1 through 8 - Select the chip to interact with. Currently selected: %i\n\r", selected_chip_number);
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"Current Clock Speed: %d\n\r", SysClkSpeed);
    UARTSend((uint8_t*) buf, strlen(buf));
    break;
  case AUTO:
    sprintf(buf,"AUTO MODE. Hit M to exit.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    break;
  default:
    UARTCharPut(UART_PRIMARY, 0xC);
    sprintf(buf,"State failure!");

  }
}

//*****************************************************************************
//
// Process changes to the menu.
//
//*****************************************************************************
void process_menu(int32_t recv_char)
{

  char buf[200];

  switch (menu_state)
  {
  case MAIN:
    switch (recv_char)
    {
    case 'm':
      menu_state = MAIN;
      break;
    case 'a':
      menu_state = AUTO;
      UARTCharPut(UART_PRIMARY, 0xC);
      break;
    case '1':
      selected_chip_number = 0;
      ChipSelect(selected_chip_number);
      break;
    case '2':
      selected_chip_number = 1;
      ChipSelect(selected_chip_number);
      break;
    case '3':
      selected_chip_number = 2;
      ChipSelect(selected_chip_number);
      break;
    case '4':
      selected_chip_number = 3;
      ChipSelect(selected_chip_number);
      break;
    case '5':
      selected_chip_number = 4;
      ChipSelect(selected_chip_number);
      break;
    case '6':
      selected_chip_number = 5;
      ChipSelect(selected_chip_number);
      break;
    case '7':
      selected_chip_number = 6;
      ChipSelect(selected_chip_number);
      break;
    case '8':
      selected_chip_number = 7;
      ChipSelect(selected_chip_number);
      break;
    }
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
//*****************************************************************************
//
// The interrupt handler for the timer interrupt.
//
//*****************************************************************************
void
Timer0IntHandler(void)
{

  //
  // Clear the timer interrupt.
  //
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  // Move onto the next cycle and trigger the timer if it's time to
  timer_current_cycle++;
  if(timer_current_cycle >= TIMER_CYCLES){
    timer_current_cycle = 0;
    timer_wakeup = true;
  }
}

int main(void)
{
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    FPUEnable();
    FPULazyStackingEnable();

    //
    // Set the clock speed. This may need a second look.
    //
    SysClkSpeed = SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_SYSDIV_1);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    ///////////////////////
    // Begin UART Config //
    ///////////////////////

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB)
    //
    // Wait for peripheral access to be enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1) ||
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
    {
    }
    //
    // Set GPIO B0 and B1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSetExpClk(UART1_BASE, SysClkSpeed, BAUD_RATE,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
    //
    // Clear the UART console
    //
    UARTCharPut(UART_PRIMARY, 0xC);


    /////////////////////
    // End UART Config //
    /////////////////////


    /////////////////////////
    // Begin IO Pin Config //
    /////////////////////////

    // This is where SPI config goes
    SysCtlPeripheralEnable(SPI0_SYS_PORT)
    SysCtlPeripheralEnable(SPI1_SYS_PORT)

    while(!SysCtlPeripheralReady(SPI0_SYS_PORT) ||
          !SysCtlPeripheralEnable(SPI1_SYS_PORT))
    {
    }
    // Board 1 SPI
    GPIOPinConfigure(SPI0_CLK);
    GPIOPinConfigure(SPI0_MOSI);
    GPIOPinConfigure(SPI0_MISO);
    GPIOPinTypeSSI(SPI0_PORT, SPI0_CLK_NUM | SPI0_MOSI_NUM | SPI0_MISO_NUM);

    SSIDisable(SPI0_NUM_BASE);
    SSIConfigSetExpClk(SPI0_NUM_BASE, SysClkSpeed, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, SPI_CLK_SPEED, 8);
    SSIEnable(SPI0_NUM_BASE);

    // Board 2 SPI
    GPIOPinConfigure(SPI1_CLK);
    GPIOPinConfigure(SPI1_MOSI);
    GPIOPinConfigure(SPI1_MISO);
    GPIOPinTypeSSI(SPI1_PORT, SPI1_CLK_NUM | SPI1_MOSI_NUM | SPI1_MISO_NUM);

    SSIDisable(SPI1_NUM_BASE);
    SSIConfigSetExpClk(SPI1_NUM_BASE, SysClkSpeed, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, SPI_CLK_SPEED, 8);
    SSIEnable(SPI1_NUM_BASE);

    //
    // Clear out the FIFOs
    //
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI0_NUM_BASE, &temp) &&
          SSIDataGetNonBlocking(SPI1_NUM_BASE, &temp))
    {
    }

    // Enable the chip select lines to the MUX, defaults to 0000
    EnableChipSelects();

    ///////////////////////
    // End IO Pin Config //
    ///////////////////////


    /////////////////////////////
    // Begin Sleep Mode Config //
    /////////////////////////////

    // Enable peripherals needed in sleep mode
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);

    // Set clock gating to false
    // This may have to be changed later
    SysCtlPeripheralClockGating(false);

    ///////////////////////////
    // End Sleep Mode Config //
    ///////////////////////////


    ////////////////////////
    // Begin Timer Config //
    ////////////////////////

    //
    // Enable the timer peripheral
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //
    // Configure a 32bit periodic timer. (Currently 1 sec)
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysClkSpeed * MEMORY_CYCLE_TIME);

    //
    // Setup the interrupts for the timer timeouts.
    //
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Enable the timers.
    //
    //TimerEnable(TIMER0_BASE, TIMER_A); Not currently used


    menu_state = MAIN;
    print_menu();

    //
    // Loop Forever
    //
    while(1)
    {
        //SysCtlSleep(); Probably necessary in space but not right now

        if(menu_state == AUTO)
            for(chip_number = 0; chip_number < TOTAL_CHIP_COUNT; chip_number++)
            {
                ChipSelect(chip_number);
                SysCtlDelay(SysClkSpeed * 1/2);
            }
    }
















}

