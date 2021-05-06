#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "io.h"
#include "at45db642.h"

int ReadFileAndProgramFlashChip(const char *FileName)
{
  FILE *FileStream;            /* File descriptor */
  struct stat FileInfo;        /* file information */
  int ReadBytes = 0, TotalReadBytes = 0;
  unsigned char ReadData[BYTES_PER_PAGE];
  unsigned short int FlashChipPageAddressToEraseAndProgram = 0;
  int SpiFlashFileDescriptor = 0, Npcs0Descriptor = 0, SpckDescriptor = 0, MosiDescriptor = 0, MisoDescriptor = 0;

  /* NULL pointer check */
  if ((void *)FileName == NULL)
  {
    return (-1);
  }

  /* let's open a file for read purpose only */
  if (((FileStream = fopen(FileName,"r")) == NULL))
  {
    return (-1);
  }

  /* get the information about file */
  if(stat(FileName, &FileInfo))
  {
    fclose(FileStream);
    return (-1);
  }

  /* Make sure that the file is an ordinary file (not symbolic link) */
  if ((!S_ISREG(FileInfo.st_mode)) || (!IsFileNameTypeValid(FileName, "bin")))
  {
    fclose(FileStream);
    return (-1);
  }

  /* Set the SPI pins of the processor to initialized state */
  if (OpenSpiFlashChip(&SpiFlashFileDescriptor, &Npcs0Descriptor, &SpckDescriptor, &MosiDescriptor, &MisoDescriptor) < 0)
  {
    /* Can't initialize the SPI pins */
    fclose(FileStream);
    return (-1);
  }

  /* Read the file by page */
  TotalReadBytes = 0;
  do
  {
    /* Seek to a defined position in a stream */
    if (fseek(FileStream, (long int)TotalReadBytes, SEEK_SET) < 0)
    {
      fclose(FileStream);
      /* Close the SPI flash chip */
      if (CloseSpiFlashChip(SpiFlashFileDescriptor, Npcs0Descriptor, SpckDescriptor, MosiDescriptor) < 0)
      {
        /* Can't reset pins to uninitialized state */
        return (-1);
      }
      return (-1);
    }

    /* Read the file as one string into the buffer */
    if ((ReadBytes = (int)fread((void *)ReadData, 1, BYTES_PER_PAGE, FileStream)) != (int)BYTES_PER_PAGE)
    {
      if (ferror(FileStream))
      {
        /* Can't read the file successfully */
        fclose(FileStream);
        /* Close the SPI flash chip */
        if (CloseSpiFlashChip(SpiFlashFileDescriptor, Npcs0Descriptor, SpckDescriptor, MosiDescriptor) < 0)
        {
          /* Can't reset pins to uninitialized state */
          return (-1);
        }
        return (-1);
      }
    }

    /* Wait until the SPI flash chip is ready */
    if (ReadSpiFlashStatusRegisterUntilChipIsReady(Npcs0Descriptor, SpckDescriptor, MosiDescriptor, MisoDescriptor) < 0)
    {
      /* Waiting time is too long for flash chip will be ready */
      fclose(FileStream);
      /* Close the SPI flash chip */
      if (CloseSpiFlashChip(SpiFlashFileDescriptor, Npcs0Descriptor, SpckDescriptor, MosiDescriptor) < 0)
      {
        /* Can't reset pins to uninitialized state */
        return (-1);
      }
      return (-1);
    }

    /* Program read data into the SPI flash */
    if (EraseAndProgramMemoryPageinsideSpiFlashChip(Npcs0Descriptor, SpckDescriptor, MosiDescriptor, MisoDescriptor, FlashChipPageAddressToEraseAndProgram, ReadData, (short int)ReadBytes) < 0)
    {
      /* Can't erase and program the SPI flash chip */
      fclose(FileStream);
      /* Close the SPI flash chip */
      if (CloseSpiFlashChip(SpiFlashFileDescriptor, Npcs0Descriptor, SpckDescriptor, MosiDescriptor) < 0)
      {
        /* Can't reset pins to uninitialized state */
        return (-1);
      }
      return (-1);
    }

    if (FlashChipPageAddressToEraseAndProgram < PAGES_PER_CHIP)
    {
      FlashChipPageAddressToEraseAndProgram++;
    }

    TotalReadBytes += (int)ReadBytes;

  } while (TotalReadBytes < (int)FileInfo.st_size);

  /* Close the SPI flash chip */
  if (CloseSpiFlashChip(SpiFlashFileDescriptor, Npcs0Descriptor, SpckDescriptor, MosiDescriptor) < 0)
  {
    /* Can't reset pins to uninitialized state */
    return (-1);
  }

  /* Close the file */
  fclose(FileStream);

  return (TotalReadBytes);
}

/* Return value: 0 - file name is not valid; otherwise - file name is valid */
int IsFileNameTypeValid(const char *FileName, const char *ValidFileNameType)
{
  unsigned int FileNameLength = 0, ValidFileNameTypeLength = 0, Position = 0;

  if ((FileName == (char *)NULL) || (ValidFileNameType == (char *)NULL))
  {
    /* File is not valid */
    return (0);
  }

  FileNameLength = (unsigned int)strlen(FileName);
  ValidFileNameTypeLength = (unsigned int)strlen(ValidFileNameType);
  if (FileNameLength <= ValidFileNameTypeLength)
  {
    /* File name is too short */
    return (0);
  }

  /* '.' check in its position */
  if (FileName[FileNameLength - ValidFileNameTypeLength - 1] != '.')
  {
    /* '.' has not been found in position before file extension */
    return (0);
  }

  /* Compare last symbols of file extension */
  for (Position = 0; Position < ValidFileNameTypeLength; Position++)
  {
    if (FileName[FileNameLength - ValidFileNameTypeLength + Position] != ValidFileNameType[Position])
    {
      /* File is not valid */
      return (0);
    }
  }

  /* File name is valid to get its content */
  return (1);
}
