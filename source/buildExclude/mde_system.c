/*
 * mde_system.c
 *
 * Contains the functions and methods that control the system to perform
 * the MDE experiment
 */


/*
*******************************************************************************
*                             Include Files                             *
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

// Custom files
#include "source/mde_system.h"
//#include "MB85RS2MTYPNF.h"
//#include "MR25H40.h"
//#include "IS62WVS5128GBLL.h"
//#include "S25FL256LA.h"









