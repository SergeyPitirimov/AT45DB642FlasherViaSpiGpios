#ifndef AT45DB642_H
#define AT45DB642_H

#define PAGES_PER_CHIP 8192
#define BYTES_PER_PAGE 1056

#define COMMAND_MAIN_MEMORY_PAGE_READ 0x52
#define COMMAND_MAIN_MEMORY_PAGE_READ_PACKET_LENGTH_IN_BYTES 9
#define COMMAND_STATUS_REGISTER_READ 0x57
#define COMMAND_STATUS_REGISTER_READ_PACKET_LENGTH_IN_BYTES 2
#define COMMAND_PAGE_ERASE 0x81
#define COMMAND_PAGE_ERASE_PACKET_LENGTH_IN_BYTES 4
#define COMMAND_BLOCK_ERASE 0x50
#define COMMAND_BLOCK_ERASE_PACKET_LENGTH_IN_BYTES 4
#define COMMAND_MAIN_MEMORY_PAGE_PROGRAM 0x82
#define COMMAND_MAIN_MEMORY_PAGE_PROGRAM_PACKET_LENGTH_IN_BYTES (BYTES_PER_PAGE + 4)

#define MAX_FLASH_CHIP_NOT_READY_TIME 0xFFFFF
#define FLASH_CHIP_IS_READY (1 << 7)

int OpenSpiFlashChip(int *SpiFlashChipFileDescriptor, int *SpiFlashChipNpcs0Descriptor, int *SpiFlashChipSpckDescriptor, int *SpiFlashChipMosiDescriptor, int *SpiFlashChipMisoDescriptor);
int ReadSpiFlashStatusRegisterUntilChipIsReady(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor);
int SendCommandThenReadDataFromSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned char *CommandToSend, unsigned short int CommandLength, unsigned char *ReadData);
int EraseMemoryPageInsideSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned short int PageAddress);
int EraseMemoryBlockInsideSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned short int BlockAddress);
int EraseAndProgramMemoryPageinsideSpiFlashChip(int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor, int SpiFlashChipMisoDescriptor, unsigned short int PageAddress, const unsigned char *DataToProgram, unsigned short int DataSizeInBytes);
int CloseSpiFlashChip(int SpiFlashChipFileDescriptor, int SpiFlashChipNpcs0Descriptor, int SpiFlashChipSpckDescriptor, int SpiFlashChipMosiDescriptor);
#endif
