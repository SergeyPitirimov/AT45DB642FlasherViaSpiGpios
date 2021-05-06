#ifndef SPI_H
#define SPI_H

/* Master clock is 60 MHz */

int OpenSpiBus(int *SpiGpioDescriptor, int *SpiNpcs0Descriptor, int *SpiSpckDescriptor, int *SpiMosiDescriptor, int *SpiMisoDescriptor);
int ActivateNpcs0(int SpiNpcs0Descriptor);
int DeactivateNpcs0(int SpiNpcs0Descriptor);
int SetSpck(int SpiSpckDescriptor);
int ClearSpck(int SpiSpckDescriptor);
int SetMosi(int SpiMosiDescriptor);
int ClearMosi(int SpiMosiDescriptor);
unsigned int ReadMiso(int SpiMisoDescriptor);
int WriteDataToSpiBus(int SpiSpckDescriptor, int SpiMosiDescriptor, unsigned char DataToWriteToSpiBus);
int WriteAndReadDataFromSpiBus(int SpiSpckDescriptor, int SpiMosiDescriptor, int SpiMisoDescriptor,  unsigned char DataToWriteToSpiBus, unsigned char *ReadSpiBusData);
int CloseSpiBus(int SpiGpioDescriptor);

#endif
