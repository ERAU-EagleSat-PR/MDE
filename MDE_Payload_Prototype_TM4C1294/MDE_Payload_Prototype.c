//*****************************************************************************
//
// The MDE payload prototype.
// Maybe we should have a license?
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "MDE_Payload_Prototype.h"
#include "Access_Tools.h"
#include "M95M02.h"
#include "CY15B104Q.h"
#include "MR25H40.h"
#include "IS62WVS5128GBLL.h"
#include "S25FL256LA.h"
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

//
// Initializing global variables
//

// Sequence variables, which control what is written to the chips
// It's best if the offset is prime
// If sequence value sets are changed, make sure to change the corresponding
// length #defines in the header file.
unsigned const char sequence_start_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
uint32_t current_sequence_start = 0;
unsigned const char sequence_offset_values[] = {7, 13, 17, 19, 23, 29, 31, 33};
uint32_t current_sequence_offset = 0;
unsigned short cycle_count = 0;

// The error buffer variables
uint64_t error_buffer[ERROR_BUFFER_MAX_SIZE];
uint32_t current_error = 0;
uint32_t old_current_error = 0;
// The unresponsiveness buffer
uint64_t chip_unresponsive;

// State trackers for the menu in debug mode
#ifdef DEBUG
enum MENU_STATES menu_state = INIT;
uint32_t selected_chip_number = 0;
#endif

// Variables for the timer
uint32_t timer_current_cycle = 0;
bool timer_wakeup = false;

// The current clock speed
uint32_t SysClkSpeed = 0;

//*****************************************************************************
//
// Transmit the current error buffer over UART.
//
//*****************************************************************************
void
TransmitErrors(){
  
#ifdef DEBUG
  
  char buf[100];
  
  sprintf(buf,"Printing errors. Current error count: %d.\n\r", current_error);
  UARTSend((uint8_t*) buf, strlen(buf));
  
  // Output if chips are unresponsive
  uint32_t unresponsive_iter;
  for(unresponsive_iter = 0; unresponsive_iter < TOTAL_CHIP_COUNT; unresponsive_iter++){
    if((chip_unresponsive >> unresponsive_iter) & 1 == 1){
      sprintf(buf, "Critical Error: Chip %i is unresponsive or wiped.\n\r", unresponsive_iter);
      UARTSend((uint8_t*) buf, strlen(buf));
    }
  }
  
  // Transmit errors over UART
  uint32_t error_iter;
  for(error_iter = 0; error_iter < current_error; error_iter++){
    sprintf(buf,"Error: %llx\n\r", error_buffer[error_iter]);
    UARTSend((uint8_t*) buf, strlen(buf));
  }
  current_error = 0;
  
#else
  
  uint32_t error_iter;
  unsigned char data1;
  unsigned char data2;
  unsigned char data3;
  for(error_iter = 0; error_iter < current_error; error_iter++){
    
    while(UARTBusy(UART_PRIMARY)){
    }
    
    // Transmit the 3 byte data header, 0b10000000/0x80
    UARTCharPut(UART_PRIMARY, 0x80);
    UARTCharPut(UART_PRIMARY, 0x80);
    UARTCharPut(UART_PRIMARY, 0x80);
    
    // Putting data into uchars for transmission
    data1 = error_buffer[error_iter] & 0xFF;
    data2 = (error_buffer[error_iter]>>8) & 0xFF;
    data3 = (error_buffer[error_iter]>>16) & 0xFF;
    
    // Transmit data
    UARTCharPut(UART_PRIMARY, data3);
    UARTCharPut(UART_PRIMARY, data2);
    UARTCharPut(UART_PRIMARY, data1);
    
  }
  current_error = 0;
#endif
  
}

//*****************************************************************************
//
// Create the health data integer and transmit it over UART.
//
//*****************************************************************************
void
TransmitHealth() {
  
  uint64_t health_data;
  
  health_data = (((uint64_t)HEALTH_DATA_HEADER_VALUE & HEALTH_DATA_HEADER_MASK) << HEALTH_DATA_HEADER_SHIFT) |
    (((uint64_t)cycle_count & HEALTH_DATA_CYCLE_MASK) << HEALTH_DATA_CYCLE_SHIFT) |
      (((uint64_t)chip_unresponsive & HEALTH_DATA_RESPONSIVENESS_MASK) << HEALTH_DATA_RESPONSIVENESS_SHIFT) |
        (((uint64_t)current_sequence_start & HEALTH_DATA_START_MASK) << HEALTH_DATA_START_SHIFT) |
          (((uint64_t)current_sequence_offset & HEALTH_DATA_OFFSET_MASK) << HEALTH_DATA_OFFSET_SHIFT);
  
#ifdef DEBUG
  
  char buf[100];
  sprintf(buf, "Health Data: %llx", health_data);
  UARTSend((uint8_t*) buf, strlen(buf));
  
#else
  
  unsigned char data0;
  unsigned char data1;
  unsigned char data2;
  unsigned char data3;     
  unsigned char data4;
  unsigned char data5;
  unsigned char data6;
  unsigned char data7;
  
  // Putting data into uchars for transmission
  data0 = health_data & 0xFF;
  data1 = (health_data>>8) & 0xFF;
  data2 = (health_data>>16) & 0xFF;
  data3 = (health_data>>24) & 0xFF;
  data4 = (health_data>>32) & 0xFF;
  data5 = (health_data>>40) & 0xFF;
  data6 = (health_data>>48) & 0xFF;
  data7 = (health_data>>54) & 0xFF;
  
  // Transmit data
  UARTCharPut(UART_PRIMARY, data7);
  UARTCharPut(UART_PRIMARY, data6);
  UARTCharPut(UART_PRIMARY, data5);
  UARTCharPut(UART_PRIMARY, data4);
  UARTCharPut(UART_PRIMARY, data3);
  UARTCharPut(UART_PRIMARY, data2);
  UARTCharPut(UART_PRIMARY, data1);
  UARTCharPut(UART_PRIMARY, data0);
  
#endif
  
}

#ifdef DEBUG

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
    sprintf(buf,"MDE Payload Prototype Interface\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    break;
  case MAIN:
    UARTCharPut(UART_PRIMARY, 0xC);
    sprintf(buf,"Menu Selection:\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"M - Return to this menu.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"P - Print the current error buffer and reset it.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"I - Enter Auto mode.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"H - Print health data.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"1 through 8 - Select the chip to interact with. Currently selected: %i\n\r", selected_chip_number);
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"All commands are listed are in order of EEPROM, Flash, FRAM, MRAM, SRAM.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"Q, W, E, R, T - send sequence to a chip.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"A, S, D, F, G - get sequence from a chip.\n\r\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"Current Clock Speed: %d\n\r", SysClkSpeed);
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf,"Current sequence parameters:\n\rStart: %i, Offset: %i\n\r\n\r", sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
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
void
process_menu(int32_t recv_char)
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
    case 'p':
      TransmitErrors();
      break;
    case 'i':
      menu_state = AUTO;
      UARTCharPut(UART_PRIMARY, 0xC);
      break;
    case 'h':
      TransmitHealth();
      break;
    case '1':
      selected_chip_number = 0;
      break;
    case '2':
      selected_chip_number = 1;
      break;
    case '3':
      selected_chip_number = 2;
      break;
    case '4':
      selected_chip_number = 3;
      break;
    case '5':
      selected_chip_number = 4;
      break;
    case '6':
      selected_chip_number = 5;
      break;
    case '7':
      selected_chip_number = 6;
      break;
    case '8':
      selected_chip_number = 7;
      break;
    case 'q':
      sprintf(buf,"Attempting to send data to EEPROM (chip %i)...\n\r", selected_chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
      WriteToChip(selected_chip_number, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break;
    case 'w':
      sprintf(buf,"Attempting to send data to Flash (chip %i)...\n\r", selected_chip_number + 8);
      UARTSend((uint8_t*) buf, strlen(buf));
      WriteToChip(selected_chip_number + 8, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break;
    case 'e':
      sprintf(buf,"Attempting to send data to FRAM (chip %i)...\n\r", selected_chip_number + 16);
      UARTSend((uint8_t*) buf, strlen(buf));
      WriteToChip(selected_chip_number + 16, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break;
    case 'r':
      sprintf(buf,"Attempting to send data to MRAM (chip %i)...\n\r", selected_chip_number + 24);
      UARTSend((uint8_t*) buf, strlen(buf));
      WriteToChip(selected_chip_number + 24, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break; 
    case 't':
      sprintf(buf,"Attempting to send data to SRAM (chip %i)...\n\r", selected_chip_number + 32);
      UARTSend((uint8_t*) buf, strlen(buf));
      WriteToChip(selected_chip_number + 32, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break; 
    case 'a':
      sprintf(buf,"Attempting to read data from EEPROM (chip %i)...\n\r", selected_chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
      ReadFromChip(selected_chip_number, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break;
    case 's':
      sprintf(buf,"Attempting to read data from Flash (chip %i)...\n\r", selected_chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
      ReadFromChip(selected_chip_number + 8, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break;
    case 'd':
      sprintf(buf,"Attempting to read data from FRAM (chip %i)...\n\r", selected_chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
      ReadFromChip(selected_chip_number + 16, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break;
    case 'f':
      sprintf(buf,"Attempting to read data from MRAM (chip %i)...\n\r", selected_chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
      ReadFromChip(selected_chip_number + 24, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      break;
    case 'g':
      sprintf(buf,"Attempting to read data from SRAM (chip %i)...\n\r", selected_chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
      ReadFromChip(selected_chip_number + 32, sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
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

#else

void
process_command(int32_t recv_char)
{
  
  switch (recv_char)
  {
  case 0x53: // transmit health data
    TransmitHealth();
    break;
  }
}
#endif

#ifdef ENABLE_UART0
//*****************************************************************************
//
// The UART0 interrupt handler.
//
//*****************************************************************************
void
UART0IntHandler(void)
{
  uint32_t ui32Status;
  
  //
  // Get the interrrupt status.
  //
  ui32Status = UARTIntStatus(UART0_BASE, true);
  
  //
  // Clear the asserted interrupts.
  //
  UARTIntClear(UART0_BASE, ui32Status);
  
  //
  // Loop while there are unsigned characters in the receive FIFO.
  //
  while(UARTCharsAvail(UART0_BASE))
  {
    
    int32_t local_char;
    local_char = UARTCharGet(UART0_BASE);
    if(local_char != -1)
    {
#ifdef DEBUG
      process_menu(local_char);
#else
      process_command(local_char);
#endif
    }
#ifdef DEBUG
    print_menu();
#endif
  }
}
#endif

#ifdef ENABLE_UART2
//*****************************************************************************
//
// The UART2 interrupt handler.
//
//*****************************************************************************
void
UART2IntHandler(void)
{
  uint32_t ui32Status;
  
  //
  // Get the interrrupt status.
  //
  ui32Status = UARTIntStatus(UART2_BASE, true);
  
  //
  // Clear the asserted interrupts.
  //
  UARTIntClear(UART2_BASE, ui32Status);
  
  //
  // Loop while there are unsigned characters in the receive FIFO.
  //
  while(UARTCharsAvail(UART2_BASE))
  {
    
    int32_t local_char;
    local_char = UARTCharGet(UART2_BASE);
    if(local_char != -1)
    {
      process_command(local_char);
    }
  }
}
#endif

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

//*****************************************************************************
//
// This is a prototype of the MDE
//
//*****************************************************************************
int
main(void)
{
  //
  // Enable lazy stacking for interrupt handlers.  This allows floating-point
  // instructions to be used within interrupt handlers, but at the expense of
  // extra stack usage.
  //
  FPUEnable();
  FPULazyStackingEnable();
  
  //
  // Set the clock speed.
  //
  SysClkSpeed = SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320, SYS_CLK_SPEED);
  
  //
  // Enable processor interrupts.
  //
  IntMasterEnable();
  
  ///////////////////////
  // Begin UART Config //
  /////////////////////// 
  
#ifdef ENABLE_UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  
  //
  // Check if the peripheral access is enabled.
  //
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0) || 
        !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
  {
  }
  
  //
  // Set GPIO A0 and A1 as UART pins.
  //
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  
  //
  // Configure the UART for 115,200, 8-N-1 operation.
  //
  UARTConfigSetExpClk(UART0_BASE, SysClkSpeed, BAUD_RATE,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  
  //
  // Enable the UART interrupt.
  //
  IntEnable(INT_UART0);
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
  
  //
  // Clear the UART console
  //
  UARTCharPut(UART_PRIMARY, 0xC);
#endif
  
#ifdef ENABLE_UART2
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  
  //
  // Check if the peripheral access is enabled.
  //
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART2) ||
        !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
  {
  }
  
  //
  // Unlock Port D for UART2
  //
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
  HWREG(GPIO_PORTD_BASE + GPIO_O_AFSEL) &= ~0x80;   
  HWREG(GPIO_PORTD_BASE + GPIO_O_DEN) |= 0x80;
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
  
  //
  // Set GPIO D6 and D7 as UART pins.
  //
  GPIOPinConfigure(GPIO_PD6_U2RX);
  GPIOPinConfigure(GPIO_PD7_U2TX);
  GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
  
  //
  // Configure the UART for 115,200, 8-N-1 operation.
  //
  UARTConfigSetExpClk(UART2_BASE, SysClkSpeed, BAUD_RATE,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  
  //
  // Enable the UART interrupt.
  //
  IntEnable(INT_UART2);
  UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT);
#endif
  
  /////////////////////
  // End UART Config //
  /////////////////////
  
  /////////////////////////
  // Begin IO Pin Config //
  /////////////////////////
  
  //
  // Enable N-PORT GPIO, SPI port, and chip ports
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  SysCtlPeripheralEnable(SPI_SYS_PORT);
  SysCtlPeripheralEnable(SPI_SYS_DAT_PORT);
  
  //
  // Check if the peripheral access is enabled.
  //
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION) ||
        !SysCtlPeripheralReady(SPI_SYS_PORT) ||
        !SysCtlPeripheralReady(SPI_SYS_DAT_PORT))
  {
  }
  
  //
  // Enable all the chip selects
  //
  EnableChipSelects();
  
  //
  // Enable the SPI interface
  //
  SysCtlPeripheralEnable(SPI_SYS_PERIPH);
  
  while(!SysCtlPeripheralReady(SPI_SYS_PERIPH))
  {
  }
  
  //
  // Configure SPI pins
  //
  GPIOPinConfigure(SPI_CLK);
  GPIOPinConfigure(SPI_MOSI);
  GPIOPinConfigure(SPI_MISO);
  GPIOPinTypeSSI(SPI_PORT, SPI_CLK_NUM | SPI_MOSI_NUM | SPI_MISO_NUM);
  
  GPIOPinConfigure(SPI_DAT2);
  GPIOPinConfigure(SPI_DAT3);
  GPIOPinTypeSSI(SPI_DAT_PORT, SPI_DAT2_NUM | SPI_DAT3_NUM);
  
  //
  // Configure SPI interface
  //
  SSIDisable(SPI_NUM_BASE);
  SSIConfigSetExpClk(SPI_NUM_BASE, SysClkSpeed, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, SPI_CLK_SPEED, 8);
  SSIEnable(SPI_NUM_BASE);
  
  //
  // Clear out the FIFO
  //
  uint32_t temp;
  while(SSIDataGetNonBlocking(SPI_NUM_BASE, &temp))
  {
  }
  
  //
  // Configure Memory Registers
  //
  /* IS THIS ALL SUPPOSED TO BE COMMENTED?
  // Removing EEPROM write restrictions
  
  // Drop CS line
  GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
  {
}
  
  // Transmit instruction to remove any write protections
  SSIDataPut(SPI_NUM_BASE, EEPROM_WRSR);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  while(SSIBusy(SPI_NUM_BASE))
  {
}
  
  // Raise CS line
  GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, EEPROM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
  {
}
  
  //
  // Removing Flash write restrictions
  //
  
  // Drop CS line
  GPIOPinWrite(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN))
  {
}
  
  // Transmit enable write status register instruction
  SSIDataPut(SPI_NUM_BASE, FLASH_EWSR);
  while(SSIBusy(SPI_NUM_BASE))
  {
}
  
  // Cycle CS line
  GPIOPinWrite(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, FLASH_CHIP_SELECT_PIN);
  while(!GPIOPinRead(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN))
  {
}
  GPIOPinWrite(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN))
  {
}
  
  // Write to the status register to remove the write restrictions
  SSIDataPut(SPI_NUM_BASE, FLASH_WRSR);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  while(SSIBusy(SPI_NUM_BASE))
  {
}
  
  // Raise CS line
  GPIOPinWrite(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, FLASH_CHIP_SELECT_PIN);
  while(!GPIOPinRead(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN))
  {
}
  
  //
  // Removing MRAM write restrictions
  //
  
  // Drop CS line
  GPIOPinWrite(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN))
  {
}
  
  // Enable writing to the register
  SSIDataPut(SPI_NUM_BASE, MRAM_WRITE_ENABLE);
  while(SSIBusy(SPI_NUM_BASE))
  {
}
  
  // Cycle the CS
  GPIOPinWrite(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN, MRAM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN))
  {
}
  GPIOPinWrite(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN))
  {
}
  
  // Write the code to remove write restrictions
  SSIDataPut(SPI_NUM_BASE, MRAM_WRSR);
  SSIDataPut(SPI_NUM_BASE, 0x02);
  while(SSIBusy(SPI_NUM_BASE))
  {
}
  
  // Cycle the CS
  GPIOPinWrite(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN, MRAM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN))
  {
}
  GPIOPinWrite(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN))
  {
}
  
  // Disable writing to the register
  SSIDataPut(SPI_NUM_BASE, MRAM_WRITE_DISABLE);
  while(SSIBusy(SPI_NUM_BASE))
  {
}
  
  // Raise CS line
  GPIOPinWrite(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN, MRAM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(MRAM_CHIP_SELECT_PORT, MRAM_CHIP_SELECT_PIN))
  {
}
  
  //
  // SRAM to sequential mode
  //
  
  // Drop CS line
  GPIOPinWrite(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN))
  {
}
  
  // Transmit instruction and code for sequential operation
  SSIDataPut(SPI_NUM_BASE, SRAM_WRMR);
  SSIDataPut(SPI_NUM_BASE, SRAM_SEQUENTIAL_CODE);
  while(SSIBusy(SPI_NUM_BASE))
  {
}
  
  // Bring CS high again
  GPIOPinWrite(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN, SRAM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN))
  {
}
  */
  ///////////////////////
  // End IO Pin Config //
  ///////////////////////
  
  /////////////////////////////
  // Begin Sleep Mode Config //
  /////////////////////////////
  
  // Enable the peripherals that we need in sleep mode
#ifdef ENABLE_UART0
  SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);
#endif
#ifdef ENABLE_UART2
  SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART2);
#endif
  SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);
  
  // Set power mode
  SysCtlSleepPowerSet(SYSCTL_FLASH_NORMAL | SYSCTL_SRAM_NORMAL);
  
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
  // Configure a 32bit periodic timer.
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
  TimerEnable(TIMER0_BASE, TIMER_A);
  
  //////////////////////
  // End Timer Config //
  //////////////////////
  
  // Wait a bit and flash some lights
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
  
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
  SysCtlDelay(SysClkSpeed * 1/3);
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
  
#ifdef DEBUG
  // Send out menu splash.
  print_menu();
#endif
  
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
  /*EEPROMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
  FlashSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset], SysClkSpeed);
  FRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
  MRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
  SRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);*/
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
  
#ifdef DEBUG
  // Change Menu State
  menu_state = MAIN;
  print_menu();
#endif
  
  //
  // Loop forever triggering payload activity every timeperiod.
  //
  while(1)
  {
    
    SysCtlSleep();
    
#ifdef DEBUG
    
    char buf[80];
    
    // Auto mode handling
    if(menu_state == AUTO && timer_wakeup){
      timer_wakeup = false;
      sprintf(buf,"Timer wakeup. Checking for errors...\n\r");
      UARTSend((uint8_t*) buf, strlen(buf));
      
      // Read the old data
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
      /*EEPROMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      FlashSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      FRAMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      MRAMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      SRAMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);*/
      TransmitHealth();
      TransmitErrors();
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
      
      // Increment the sequence
      current_sequence_start++;
      current_sequence_offset++;
      cycle_count++;
      if(current_sequence_start >= SEQUENCE_START_VALUES_LENGTH){
        current_sequence_start = 0;
      }
      if(current_sequence_offset >= SEQUENCE_OFFSET_VALUES_LENGTH){
        current_sequence_offset = 0;
      }
      
      // Transmit new data
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
      /*EEPROMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      FlashSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset], SysClkSpeed);
      FRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      MRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      SRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);*/
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
      
      sprintf(buf,"Sequence written, beginning next cycle.\n\r");
      UARTSend((uint8_t*) buf, strlen(buf));
    }
#else
    // Basically auto mode but for flight config
    // Might want to add another condition here to wait for a command before
    // starting or something
    if(timer_wakeup){
      timer_wakeup = false;
      
      // Read the old data
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
      /*EEPROMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      FlashSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      FRAMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      MRAMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      SRAMSequenceRetrieve(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);*/
      TransmitErrors();
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
      
      // Increment the sequence
      current_sequence_start++;
      current_sequence_offset++;
      cycle_count++;
      if(current_sequence_start >= SEQUENCE_START_VALUES_LENGTH){
        current_sequence_start = 0;
      }
      if(current_sequence_offset >= SEQUENCE_OFFSET_VALUES_LENGTH){
        current_sequence_offset = 0;
      }
      
      // Transmit new data
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
      /*EEPROMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      FlashSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset], SysClkSpeed);
      FRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      MRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);
      SRAMSequenceTransmit(sequence_start_values[current_sequence_start], sequence_offset_values[current_sequence_offset]);*/
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
      
    }
#endif
  }
}
