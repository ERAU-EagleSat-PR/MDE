/*
 * This file is part of the EagleSat-2 Cube Satellite Project.
 *
 * This file defines constants and functions for handling the bit flip erros found
 * in the Memory Degredation Experiment experiment.
 */

#include "inc/hw_types.h"


//-----------------------------------------------------------------------------
// TransmitErrors() Error Buffer Variables //TODO
//-----------------------------------------------------------------------------

// The maximum amount of errors that can be stored
// If this is exceeded then the system loops back around, though the looping is
// sort of broken as when transmitting the system will forget about the previous
// set of errors.
#define ERROR_BUFFER_MAX_SIZE 1000

// The buffer variables
extern uint64_t error_buffer[ERROR_BUFFER_MAX_SIZE];
extern uint32_t current_error;
extern uint32_t old_current_error;


