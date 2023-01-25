//*****************************************************************************
//
// This header file contains the hardware defines, mappings, and variables to
// every memory chip and MCO peripheral
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef ACCESS_TOOLS_H
#define ACCESS_TOOLS_H

// Method prototypes
extern void WriteToChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
extern void ReadFromChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);


//*****************************************
// System Hardware Variables 
//*****************************************



//*****************************************
// Chip Select Defines with the Multiplexer 
//*****************************************

// The highest chip number that we consider valid. Since chip numbers go from 0
// to this, it's going to be the (total number of chips) - 1. Only changing
// this does not handle higher numbers of chips. Changes must also be made to
// the retrieve ports and pins functions.
#define MAX_CHIP_NUMBER 15

// GPIO Ports to enable for chip selects, hard-coded to enable each in the
// EnableChipSelects method. If another port needs to be enabled, add a macro
// here and enable in that method.

// SSI pins for MOSI/MISO board communication



#endif
