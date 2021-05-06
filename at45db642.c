#include <limits.h>
#include "at45db642.h"
#include "spi.h"

/* This function initializes the SPI module in the processor */
/* Input: Chip select line number and mode of the SPI module in processor */
/* Output: 0 - ok, -1 - error */
int OpenSpiFlashChip(int *SpiFlashChipFileDescriptor, int *SpiFlashChipNpcs0Descriptor, int *SpiFlashChipSpckDescriptor, int *SpiFlashChipMosiDescriptor, int *SpiFlashChipMisoDescriptor)
{
  /* Check the incoming data */
  if (((void *)SpiFlashChipFileDescriptor == (void *)0) || ((void *)SpiFlashChipNpcs0Descriptor == (void *)0) || ((void *)SpiFlashChipSpckDescriptor == (void *)0) || ((void *)SpiFlashChipMosiDescriptor == (void *)0) || ((void *)SpiFlashChipMisoDescriptor == (void *)0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Open the microprocessor SPI bus */
  if (OpenSpiBus(SpiFlashChipFileDescriptor, SpiFlashChipNpcs0Descriptor, SpiFlashChipSpckDescriptor, SpiFlashChipMosiDescriptor, SpiFlashChipMisoDescriptor) < 0)
  {
    /* Can't open the SPI bus */
    return (-1);
  }

  /* Deactivate the SPI chip select pin */
  ClearSpck(*SpiFlashChipSpckDescriptor);
  ClearMosi(*SpiFlashChipMosiDescriptor);
  DeactivateNpcs0(*SpiFlashChipNpcs0Descriptor);

  /* Wait until the SPI flash chip is ready */
  if (ReadSpiFlashStatusRegisterUntilChipIsReady(*SpiFlashChipNpcs0Descriptor, *SpiFlashChipSpckDescriptor, *SpiFlashChipMosiDescriptor, *SpiFlashChipMisoDescriptor) < 0)
  {
    /* Waiting time is too long for flash chip will be ready */
    if (CloseSpiFlashChip(*SpiFlashChipFileDescriptor, *SpiFlashChipNpcs0Descriptor, *SpiFlashChipSpckDescriptor, *SpiFlashChipMosiDescriptor) < 0)
    {
      return (-1);
    }
    return (-1);
  }

  /* Successfull exit */
  return 0;
}

/* This function reads the SPI flash chip status register until chip is ready for next operation */
/* Return: 0 - ok, -1 - error, which ocurrs when maximum waiting time is over */
int ReadSpiFlashStatusRegisterUntilChipIsReady(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor)
{
  unsigned char ReadSpiFlashStatusRegisterCommand[COMMAND_STATUS_REGISTER_READ_PACKET_LENGTH_IN_BYTES];
  unsigned int FlashChipStatusReadCounter = 0;
  unsigned char FlashChipStatusRegisterValue = 0;

  /* Check the incoming data */
  if ((SpiFlashChipNpcs0Descriptor < 0) || (SpiFlashChipSpckDescriptor < 0) || (SpiFlashChipMosiDescriptor < 0) || (SpiFlashChipMisoDescriptor < 0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Command to read the status register of the SPI flash chip */
  ReadSpiFlashStatusRegisterCommand[0] = COMMAND_STATUS_REGISTER_READ;
  ReadSpiFlashStatusRegisterCommand[1] = 0x0;

  do
  {
    /* Check the flash chip ready state */
    if (SendCommandThenReadDataFromSpiFlashChip(SpiFlashChipNpcs0Descriptor, SpiFlashChipSpckDescriptor, SpiFlashChipMosiDescriptor, SpiFlashChipMisoDescriptor, ReadSpiFlashStatusRegisterCommand, COMMAND_STATUS_REGISTER_READ_PACKET_LENGTH_IN_BYTES, &FlashChipStatusRegisterValue) < 0)
    {
      /* Can't read the status register of flash chip */
      return (-1);
    }

    /* Check the maximum cycle counts */
    if ((FlashChipStatusReadCounter >= MAX_FLASH_CHIP_NOT_READY_TIME) || (FlashChipStatusReadCounter >= UINT_MAX))
    {
      /* Waiting time is too long for flash chip will be ready */
      return (-1);
    }

    if (FlashChipStatusReadCounter < UINT_MAX)
    {
      FlashChipStatusReadCounter++;
    }
  } while ((FlashChipStatusRegisterValue & FLASH_CHIP_IS_READY) == 0);

  /* Successfull exit */
  return 0;
}

/* This function sends the command to the SPI flash and reads the data by page address (13 bit) and byte address (11 bit) */
/* Input: Command to send to the SPI flash chip, Command length in bytes, Page address and byte address to read from */
/* Output: byte of data from SPI flash chip */
/* Return: 0 - ok, -1 - error */
int SendCommandThenReadDataFromSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned char *CommandToSend, unsigned short int CommandLength, unsigned char *ReadData)
{
  unsigned short int ByteCounter = 0x0;

  if ((SpiFlashChipNpcs0Descriptor < 0) || (SpiFlashChipSpckDescriptor < 0) || (SpiFlashChipMosiDescriptor < 0) || (SpiFlashChipMisoDescriptor < 0) || ((void *)CommandToSend == (void *)0) || (CommandLength == 0) || ((void *)ReadData == (void *)0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Activate the SPI chip select pin */
  ClearSpck(SpiFlashChipSpckDescriptor);
  ClearMosi(SpiFlashChipMosiDescriptor);
  ActivateNpcs0(SpiFlashChipNpcs0Descriptor);

  /* Send CommandLength bytes to the flash chip and read one byte after that with result data */
  for (ByteCounter = 0; ByteCounter < (CommandLength - 1); ByteCounter++)
  {
    /* Write the byte by byte to the flash chip and read the answer after that */
    if (WriteDataToSpiBus(SpiFlashChipSpckDescriptor, SpiFlashChipMosiDescriptor, CommandToSend[ByteCounter]) < 0)
    {
      /* Can't write the data to the SPI bus */
      ClearSpck(SpiFlashChipSpckDescriptor);
      ClearMosi(SpiFlashChipMosiDescriptor);
      DeactivateNpcs0(SpiFlashChipNpcs0Descriptor);
      return (-1);
    }
  }

  /* Read the answer of SPI flash chip */
  if (WriteAndReadDataFromSpiBus(SpiFlashChipSpckDescriptor, SpiFlashChipMosiDescriptor, SpiFlashChipMisoDescriptor, CommandToSend[CommandLength - 1], ReadData) < 0)
  {
    /* Can't read the data from SPI flash chip */
    ClearSpck(SpiFlashChipSpckDescriptor);
    ClearMosi(SpiFlashChipMosiDescriptor);
    DeactivateNpcs0(SpiFlashChipNpcs0Descriptor);
    return (-1);
  }

  /* Deactivate the SPI chip select pin */
  ClearSpck(SpiFlashChipSpckDescriptor);
  ClearMosi(SpiFlashChipMosiDescriptor);
  DeactivateNpcs0(SpiFlashChipNpcs0Descriptor);

  /* Successfull exit */
  return 0;
}

/* This function erases the page of memory with PageAddress */
/* Return: 0 - ok, -1 - error */
int EraseMemoryPageInsideSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned short int PageAddress)
{
  unsigned char Command[COMMAND_PAGE_ERASE_PACKET_LENGTH_IN_BYTES];
  unsigned char ReadSpiFlashData = 0;

  /* Check the incoming data */
  if ((SpiFlashChipNpcs0Descriptor < 0) || (SpiFlashChipSpckDescriptor < 0) || (SpiFlashChipMosiDescriptor < 0) || (SpiFlashChipMisoDescriptor < 0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Fill the byte array to send to the flash chip via SPI bus */
  Command[0] = COMMAND_PAGE_ERASE;
  Command[1] = (unsigned char)((PageAddress >> 5) & 0xFF);
  Command[2] = (unsigned char)((PageAddress << 3) & 0xF8);
  Command[3] = 0x0;

  /* Send the command to erase memory page, defined by page address */
  if (SendCommandThenReadDataFromSpiFlashChip(SpiFlashChipNpcs0Descriptor, SpiFlashChipSpckDescriptor, SpiFlashChipMosiDescriptor, SpiFlashChipMisoDescriptor, Command, COMMAND_PAGE_ERASE_PACKET_LENGTH_IN_BYTES, &ReadSpiFlashData) < 0)
  {
    /* Can't erase the memory page inside the SPI flash chip */
    return (-1);
  }

  /* Successfull exit */
  return 0;
}

/* This function erases the block of memory pages (1 block = 8 pages) by BlockAddress */
/* Return: 0 - ok, -1 - error */
int EraseMemoryBlockInsideSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned short int BlockAddress)
{
  unsigned char Command[COMMAND_BLOCK_ERASE_PACKET_LENGTH_IN_BYTES];
  unsigned char ReadSpiFlashData = 0;

  /* Check the incoming data */
  if ((SpiFlashChipNpcs0Descriptor < 0) || (SpiFlashChipSpckDescriptor < 0) || (SpiFlashChipMosiDescriptor < 0) || (SpiFlashChipMisoDescriptor < 0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Fill the byte array to send to the flash chip via SPI bus */
  Command[0] = COMMAND_BLOCK_ERASE;
  Command[1] = (unsigned char)((BlockAddress >> 5) & 0xFF);
  Command[2] = (unsigned char)((BlockAddress << 3) & 0xC0);
  Command[3] = 0x0;

  /* Send the command to erase memory page, defined by page address */
  if (SendCommandThenReadDataFromSpiFlashChip(SpiFlashChipNpcs0Descriptor, SpiFlashChipSpckDescriptor, SpiFlashChipMosiDescriptor, SpiFlashChipMisoDescriptor, Command, COMMAND_BLOCK_ERASE_PACKET_LENGTH_IN_BYTES, &ReadSpiFlashData) < 0)
  {
    /* Can't erase the memory block inside the SPI flash chip */
    return (-1);
  }

  /* Successfull exit */
  return 0;
}

/* This function erases the main memory page, defined by PageAddress, and then program this page by DataToProgram */
/* Return: 0 - ok, -1 - error */
int EraseAndProgramMemoryPageinsideSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned short int PageAddress, const unsigned char *DataToProgram, unsigned short int DataSizeInBytes)
{
  unsigned char Command[COMMAND_MAIN_MEMORY_PAGE_PROGRAM_PACKET_LENGTH_IN_BYTES];
  unsigned char ReadSpiFlashData = 0;
  unsigned short int i = 0;

  /* Check the incoming data */
  if ((SpiFlashChipNpcs0Descriptor < 0) || (SpiFlashChipSpckDescriptor < 0) || (SpiFlashChipMosiDescriptor < 0) || (SpiFlashChipMisoDescriptor < 0) || ((void *)DataToProgram == (void *)0) || (DataSizeInBytes == 0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Fill the byte array to send to the flash chip via SPI bus */
  Command[0] = COMMAND_MAIN_MEMORY_PAGE_PROGRAM;
  Command[1] = (unsigned char)((PageAddress >> 5) & 0xFF);
  Command[2] = (unsigned char)((PageAddress << 3) & 0xF8);
  Command[3] = 0x0;

  for (i = 0; i < BYTES_PER_PAGE; i++)
  {
    if (i < DataSizeInBytes)
    {
      Command[4 + i] = DataToProgram[i];
    }
    else
    {
      Command[4 + i] = 0xFF;
    }
  }

  /* Send the command to erase and program the main memory page, defined by page address, buffer address and data to program */
  if (SendCommandThenReadDataFromSpiFlashChip(SpiFlashChipNpcs0Descriptor, SpiFlashChipSpckDescriptor, SpiFlashChipMosiDescriptor, SpiFlashChipMisoDescriptor, Command, COMMAND_MAIN_MEMORY_PAGE_PROGRAM_PACKET_LENGTH_IN_BYTES, &ReadSpiFlashData) < 0)
  {
    /* Can't erase and program the main memory page inside the SPI flash chip */
    return (-1);
  }

  /* Successfull exit */
  return 0;
}

/* This function disables the SPI module in the processor with depending pins */
/* Return: 0 - ok, -1 - error */
int CloseSpiFlashChip(int SpiFlashChipFileDescriptor, int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor)
{
  /* Check the incoming data */
  if ((SpiFlashChipFileDescriptor < 0) || (SpiFlashChipNpcs0Descriptor < 0) || (SpiFlashChipSpckDescriptor < 0) || (SpiFlashChipMosiDescriptor < 0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Deactivate the SPI chip select pin */
  ClearSpck(SpiFlashChipSpckDescriptor);
  ClearMosi(SpiFlashChipMosiDescriptor);
  DeactivateNpcs0(SpiFlashChipNpcs0Descriptor);

  if (CloseSpiBus(SpiFlashChipFileDescriptor) < 0)
  {
    /* Can't close the SPI bus */
    return (-1);
  }

  /* Successfull exit */
  return 0;
}
