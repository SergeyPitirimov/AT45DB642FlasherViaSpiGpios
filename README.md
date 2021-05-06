This free software allows to erase and then program Microchip/Atmel AT45DB642 SPI NAND (Dataflash) integrated circuit from file, which contains Linux kernel, root file system and device tree blob.
Usage: flasher KernelWithRootfsWithDTB.bin
Programming procedure uses GPIO pins (NCS0, SPCK, MOSI, MISO) without hardware SPI subsystem inside the processor AT91RM9200 and takes about fifty minutes for completion.
