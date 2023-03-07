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
#include "bit_errors.h"

typedef struct {
    uint8_t chip_id;
    uint16_t cell_address;
    uint8_t written_sequence;
    uint8_t retrieved_sequence;
} MDE_Error_Data_Struct;

void
CheckErrors(uint32_t byte_data, uint32_t current_sequence, uint32_t byte_num, uint8_t chip_number)
{

// removed header info, if some reason still needed can be found in breadboarding branch

    // Compare the received data to the sequence and create the error codes
    unsigned char wrong_bits = byte_data ^ current_sequence;
    if(wrong_bits != 0){

        struct MDE_Error_Data_Struct *found_error = malloc(sizeof(struct MDE_Error_Data_Struct));

        found_error->chip_id = chip_number;
        found_error->cell_address = byte_num;
        found_error->written_sequence = current_sequence;
        found_error->retrieved_sequence = byte_data;

        if(current_error <= ERROR_BUFFER_MAX_SIZE){
            error_buffer[current_error] = &found_error;
            current_error++;
        }
        // Removed bit-wise error checking as the struct does not hold that, can be found in breadboarding branch if necessary
    }
}


