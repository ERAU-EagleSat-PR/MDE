/*
 * This file is part of the EagleSat-2 Cube Satellite Project.
 *
 * This file defines constants and functions for handling the bit flip erros found
 * in the Memory Degredation Experiment experiment.
 */

#include "inc/hw_types.h"

#ifndef SOURCE_BIT_ERRORS_H_
#define SOURCE_BIT_ERRORS_H_

//
// Error queue definitions
//

// MDE Error Struct/Queue Link
typedef struct MDE_Error_Data MDE_Error_Data_t;
struct MDE_Error_Data {
    uint8_t chip_id;             // Chip number
    uint16_t cell_address;       // Byte address
    uint8_t written_sequence;    // Written sequence (all 0s or all 1s)
    uint8_t retrieved_sequence;  // Read sequence
    MDE_Error_Data_t *next; // Pointer to the next link.
};

// Queue status enum
typedef enum {
    errorsEmpty = 0,
    errorsNotEmpty,
} ErrorStatus_enum;

// Pointer to queue head
extern MDE_Error_Data_t *errorHead;

//
// MDE Functions
//
void CheckErrors(uint8_t chip_number, uint32_t byte_num, uint8_t byte_data, uint8_t written_data);

//
// Error Queue Functions
//
// Circle queue for error storage
MDE_Error_Data_t *ErrorQueue_Init(uint8_t chip_number, uint32_t byte_num, uint8_t byte_data, uint8_t written_data);
uint8_t ErrorQueue_Insert(MDE_Error_Data_t **ptr, uint8_t chip_number, uint32_t byte_num, uint8_t byte_data, uint8_t written_data);

ErrorStatus_enum ErrorQueue_IsEmpty(MDE_Error_Data_t *ptr);

void ErrorQueue_Destroy(MDE_Error_Data_t **ptr);
uint8_t ErrorQueue_Remove(MDE_Error_Data_t **ptr);

//-----------------------------------------------------------------------------
// TransmitErrors() Error Buffer Variables //TODO
//-----------------------------------------------------------------------------

// Maximum error storage based on given heap size of 16384 bytes, sizeof(MDE_Error_Data_t) = 12 bytes.
// Given buffer of 1200, there is 2384 bytes of margin.
// This value can likely be massively increased if wanted as we have 256kb of total memory, most of which is likely unused.
#define ERROR_BUFFER_MAX_SIZE 1200

// The buffer variables
extern uint32_t current_error;
extern uint32_t old_current_error;


#endif // Bit_errors.h
