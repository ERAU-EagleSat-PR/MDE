//*****************************************************************************
//
// This header file contains all the global variables and defines for the
// experiment
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef TM4C123GXL_SYSTEM_H
#define TM4C123GXL_SYSTEM_H

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/

extern void WriteToChip(    uint32_t chipNumber,
                            unsigned char sequence_start,
                            unsigned char sequence_offset);

extern void ReadFromChip(   uint32_t chipNumber,
                            unsigned char sequence_start,
                            unsigned char sequence_offset);



//*****************************************
// System Hardware Variables
//*****************************************


// The highest chip number that we consider valid. Since chip numbers go from 0
// to this, it's going to be the (total number of chips) - 1. Only changing
// this does not handle higher numbers of chips. Changes must also be made to
// the retrieve ports and pins functions.
#define MAX_CHIP_NUMBER     15


#endif
