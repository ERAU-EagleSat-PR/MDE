/*
 * This file is part of the EagleSat-2 Cube Satellite Project.
 *
 * This file defines constants and functions for handling the bit flip erros found
 * in the Memory Degredation Experiment experiment.
 */

#include "inc/hw_types.h"

#ifndef SOURCE_BIT_ERRORS_H_
#define SOURCE_BIT_ERRORS_H_

// Data structure for temporary storage and transmission of errors.
typedef struct {
    uint8_t chip_id;            // Chip number
    uint16_t cell_address;      // Byte address
    uint8_t written_sequence;   // Written sequence (all 0s or all 1s)
    uint8_t retrieved_sequence; // Read sequence
} MDE_Error_Data;

void CheckErrors(uint8_t chip_number, uint32_t byte_num, uint8_t byte_data, uint8_t data_cycle);

//-----------------------------------------------------------------------------
// TransmitErrors() Error Buffer Variables //TODO
//-----------------------------------------------------------------------------

// The maximum amount of errors that should be stored in one cycle due to memory concerns.
// TODO: Perhaps to be a bit more careful we could include a counter to track if errors are counted in excess of this buffer during a cycle.
//       would need to find some way to send it to OBC though, may be impractical.
#define ERROR_BUFFER_MAX_SIZE 1000

// The buffer variables
extern MDE_Error_Data *error_buffer[ERROR_BUFFER_MAX_SIZE];
extern uint32_t current_error;
extern uint32_t old_current_error;


#endif // Bit_errors.h
