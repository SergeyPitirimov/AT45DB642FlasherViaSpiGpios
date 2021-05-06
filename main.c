#include <stdio.h>
#include <stdlib.h>
#include "io.h"

int main(int argc, char *argv[])
{
  char *FileName = (char *)NULL;
  int ProgrammedBytes = 0;

  /* Check that file name to program is here as parameter */
  if (argc < 2)
  {
    fprintf(stderr, "Usage: flasher <kernelwrootfswdtb.bin>.\n");
    return (-1);
  }

  fprintf(stderr, "The SPI flash chip programming started successfully. Please wait about fifty minutes for completion.\n");

  /* Get the file name to program */
  FileName = argv[1];

  /* Read the file to program page by page, then erase and program SPI flash chip */
  if ((ProgrammedBytes = ReadFileAndProgramFlashChip(FileName)) < 0)
  {
    return (-1);
  }

  fprintf(stderr, "The SPI flash chip is successfully programmed with %d bytes.\n", ProgrammedBytes);

  /* Successfull exit */
  return 0;
}
