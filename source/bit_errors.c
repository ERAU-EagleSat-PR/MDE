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

// Project files
#include "source/bit_errors.h"


void
CheckErrors(uint8_t chip_number, uint32_t byte_num, uint8_t byte_data, uint8_t written_data)
{
    // Chip number 0 - 31, byte number is current address,
    // byte data is what was read, written data is 255 or 0 only

    // Compare the received data to the cycle which was written
    unsigned char wrong_bits = byte_data ^ written_data;
    if(wrong_bits != 0){
        // Check to not exceed error limit for memory concerns
        if(current_error <= ERROR_BUFFER_MAX_SIZE){

            MDE_Error_Data *found_error = malloc(sizeof(MDE_Error_Data)); // To be free'd in OBC transmission
            found_error->chip_id = chip_number;
            found_error->cell_address = byte_num;
            found_error->written_sequence = written_data;
            found_error->retrieved_sequence = byte_data;

            error_buffer[current_error] = &found_error;
            current_error++;
        }
    }
}


