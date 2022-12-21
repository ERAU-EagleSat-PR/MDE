//*****************************************************************************
//
// The MDE payload prototype for Integration with OBC
// THis version has no scientific functionality, focuses on UART communication
// to of health and data packets to the OBC
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "MDE_OBC_integration.h"
#include "Access_Tools.h"
//#include "MB85RS2MTYPNF.h"
//#include "MR25H40.h"
//#include "IS62WVS5128GBLL.h"
//#include "S25FL256LA.h"
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

// Variables for the timer
uint32_t timer_current_cycle = 0;
bool timer_wakeup = false;

// The current clock speed
uint32_t SysClkSpeed = 0;


//*****************************************************************************
//
// DEGUGGING FUNCTIONS
//
//*****************************************************************************
#ifdef DEBUG

//*************************************
// Print the current debuf/ develpment menu configuration
//*************************************
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
    sprintf(buf,"I - Enter Auto mode.\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf, "H - Send Health Packet (TransmitHealth) \n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
    sprintf(buf, "D - Send Data Packet (TransmitErrors) \n\r");
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
// Transmit the current error buffer over UART.
//
//*****************************************************************************
/*
void TransmitErrors(){
  
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
  
}

*/

//*****************************************************************************
//
// Create the health data integer and transmit it over UART.
//
//*****************************************************************************
/*
void TransmitHealth(){
  
  uint64_t health_data;
  
  //Heath_data is a mask
  health_data = (((uint64_t)HEALTH_DATA_HEADER_VALUE & HEALTH_DATA_HEADER_MASK) << HEALTH_DATA_HEADER_SHIFT) |
    (((uint64_t)cycle_count & HEALTH_DATA_CYCLE_MASK) << HEALTH_DATA_CYCLE_SHIFT) |
      (((uint64_t)chip_unresponsive & HEALTH_DATA_RESPONSIVENESS_MASK) << HEALTH_DATA_RESPONSIVENESS_SHIFT) |
        (((uint64_t)current_sequence_start & HEALTH_DATA_START_MASK) << HEALTH_DATA_START_SHIFT) |
          (((uint64_t)current_sequence_offset & HEALTH_DATA_OFFSET_MASK) << HEALTH_DATA_OFFSET_SHIFT);
  
    #ifdef DEBUG
  
  char buf[100];
  sprintf(buf, "Health Data: %llx", health_data);
  UARTSend((uint8_t*) buf, strlen(buf));
  
#endif
  
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
  
}
*/

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
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

#ifdef ENABLE_UART0
//*****************************************************************************
//
// The UART0 interrupt handler. Debugging UART
//
//*****************************************************************************
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

#ifdef ENABLE_UART1
//*****************************************************************************
//
// The UART0 interrupt handler. UART 1 is the is the UART for OBC communication
//
//*****************************************************************************
/*
void UART1IntHandler(void)
{
  uint32_t ui32Status;
  
  //
  // Get the interrupt status.
  //
  ui32Status = UARTIntStatus(UART_PRIMARY, true);
  
  //
  // Clear the asserted interrupts.
  //
  UARTIntClear(UART_PRIMARY, ui32Status);
  
  //
  // Loop while there are unsigned characters in the receive FIFO.
  //
  while(UARTCharsAvail(UART_PRIMARY))
  {
    
    int32_t local_char;
    local_char = UARTCharGet(UART_PRIMARY);
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
*/


//*****************************************************************************
//
// The interrupt handler for the timer interrupt.
//
//*****************************************************************************
void Timer0IntHandler(void)
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
// Enables the RGB LED
//*****************************************************************************
void EnableLED(void){
  
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


//*****************************************************************************
//
// MAIN: Prototype for developing and testing integration with OBC
//
//*****************************************************************************
int main(void){
  
  //*****************************************************************************
  // Normal CEC322 stuff (Enabling peripherals and pins and stuff)
  //****************************************************************************

  // Enable lazy stacking for interrupt handlers.  This allows floating-point
  // instructions to be used within interrupt handlers, but at the expense of
  // extra stack usage.
  FPUEnable();
  FPULazyStackingEnable();

  //
  // Set the clock speed.
  //
  SysClkSpeed = SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320, SYS_CLK_SPEED);

  // Enable processor interrupts.
  IntMasterEnable();

  ////////////////////////
  // LED enable and config
  ////////////////////////
  EnableLED();

  ///////////////////////////////
  // Debug UART enable and config
  ///////////////////////////////

  // UART DEBUG

  ////////////////////////
  // Begin While Loop   //
  ////////////////////////
  while(1){

      // Idle "heart beat"
      BlinkBlueLED();
  }

}
