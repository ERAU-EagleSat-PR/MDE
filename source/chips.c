/*
 * chips.c
 *
 *  Created on: Dec 29, 2022
 *      Author: Calvin
 */



//*****************************************************************************
//
// Writes the given sequence to the chip indicated by chip number.
//
//*****************************************************************************
// TODO
/*
void WriteToChips(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset)
{

  if(chip_number > MAX_CHIP_NUMBER){
    #ifdef DEBUG
      char buf[80];
      sprintf(buf,"Tried to write to chip %i but no such chip exists!\n\r", chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
    #endif
      return;
  }

  uint32_t chip_port = RetrieveChipPort(chip_number);
  uint8_t chip_mux = RetriveChipMuxCode(chip_number % 16);

  //drip chip select here (look at the chip select sequence in each of the sequence tranits)

  // TODO  Change the transmit function for multiplexer (TYLER)
  // for each board the chip numbering is the same
  if( (chip_number % 16) < 4){
    FlashSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if( (chip_number % 16) < 8){
    FRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if( (chip_number % 16) < 12){
    MRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if( (chip_number % 16) < 16){
    SRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }
}
*/


//*****************************************************************************
//
// Reads and checks the sequence from a chip against the given sequence for the
// chip indicated by chip number.
//
//*****************************************************************************
// TODO
/*
void ReadFromChips(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset)
{

  if(chip_number > MAX_CHIP_NUMBER){
#ifdef DEBUG
    char buf[80];
    sprintf(buf,"Tried to read from chip %i but no such chip exists!\n\r", chip_number);
    UARTSend((uint8_t*) buf, strlen(buf));
#endif
    return;
  }

  uint32_t chip_port = RetrieveChipPort(chip_number);
  uint32_t chip_pin = RetrieveChipPin(chip_number);

  if (chipNumber < 4) {
      FlashSequenceRetrieve(sequence_start, sequence_offset, chip_number);
  } else if (chipNumber < 8) {
      FRAMSequenceRetrieve(sequence_start, sequence_offset, chip_number)
  } else if (chipNumber < 12) {
      MRAMSequenceRetrieve(sequence_start, sequence_offset, chip_number)
  } else {
      SRAMSequenceRetrieve(sequence_start, sequence_offset, chip_number)
  }
}
*/
