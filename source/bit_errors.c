// Standard Includes
#include <stdlib.h>
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

// Data structure for temporary storage and transmission of errors.

/*
*******************************************************************************
*                               MDE Error Functions                           *
*******************************************************************************
*/
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

            // Request new error link.
            uint8_t check = ErrorQueue_Insert(&errorHead, chip_number, byte_num, byte_data, written_data);
        }
    }
}

/*
*******************************************************************************
*                          Circle Queue For Errors                            *
*******************************************************************************
*/

MDE_Error_Data_t *ErrorQueue_Init(uint8_t chip_number, uint32_t byte_num, uint8_t byte_data, uint8_t written_data) {
//  Initialize the first link in the queue, enter data,
//  and return a pointer to it's head.

    MDE_Error_Data_t *head = (MDE_Error_Data_t *) malloc(sizeof(MDE_Error_Data_t));
    // Allocation failure check
    if (head == NULL) {
        return NULL;
    }
    // Write data to link
    head->chip_id = chip_number;
    head->cell_address = byte_num;
    head->written_sequence = written_data;
    head->retrieved_sequence = byte_data;
    head->next = head;
    // Return pointer to new link
    return head;
}

uint8_t ErrorQueue_Insert(MDE_Error_Data_t **ptr, uint8_t chip_number, uint32_t byte_num,
                        uint8_t byte_data, uint8_t written_data) {
    // Create a new link and insert it at the start of the queue.
    // 1 = success, 0 = failure
    if (ptr == NULL) { // Invalid NULL pointer
        return 0;
    }
    if (ErrorQueue_IsEmpty(*ptr) == errorsEmpty) { // Initialize empty queue
        *ptr = ErrorQueue_Init(chip_number, byte_num, byte_data, written_data);
        if(ptr == NULL) return 0;
        current_error++;
        return 1;
    }
    else { // Add link to existing queue
        MDE_Error_Data_t *new = ErrorQueue_Init(chip_number, byte_num, byte_data, written_data);
        if(new == NULL) return 0;
        new->next = (*ptr)->next;
        (*ptr)->next = new;
        (*ptr) = (*ptr)->next;
        current_error++;
        return 1;
    }
}

ErrorStatus_enum ErrorQueue_IsEmpty(MDE_Error_Data_t *ptr) {
    // Return 0 or 1 if queue is empty or full (as defined by enum in .h)
    ErrorStatus_enum test = (ptr == NULL) ?  errorsEmpty : errorsNotEmpty;
    return test;
}

uint8_t ErrorQueue_Remove(MDE_Error_Data_t **ptr) {
    // Delete a link and free it's memory. Return a 1 on success, and a 0 on failure.
    if (ptr == NULL) {
        return 0;
    }
    if (ErrorQueue_IsEmpty(*ptr) == errorsEmpty) {
        return 0;
    }
    // Re-form list without link
    MDE_Error_Data_t *remove = (*ptr)->next;
    if (*ptr == (*ptr)->next) *ptr = NULL;
    else (*ptr)->next = (*ptr)->next->next;
    // Free associated memory
    free(remove);
    current_error--;
    // Return success
    return 1;
}

void ErrorQueue_Destroy(MDE_Error_Data_t **ptr) {
    // Delete the entire queue.
    while (ErrorQueue_IsEmpty(*ptr) == errorsNotEmpty) {
        ErrorQueue_Remove(ptr);
    }
    current_error = 0;
}



