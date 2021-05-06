#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include "spi.h"

int OpenSpiBus(int *SpiGpioDescriptor, int *SpiNpcs0Descriptor, int *SpiSpckDescriptor, int *SpiMosiDescriptor, int *SpiMisoDescriptor)
{
  int GpioDescriptor = 0;
  struct gpiohandle_request MisoRequest;
  struct gpiohandle_request MosiRequest;
  struct gpiohandle_request SpckRequest;
  struct gpiohandle_request Npcs0Request;
  struct gpiohandle_data MosiData;
  struct gpiohandle_data SpckData;
  struct gpiohandle_data Npcs0Data;

  /* Check the incoming data */
  if (((void *)SpiGpioDescriptor == (void *)0) || ((void *)SpiNpcs0Descriptor == (void *)0) || ((void *)SpiSpckDescriptor == (void *)0) || ((void *)SpiMosiDescriptor == (void *)0) || ((void *)SpiMisoDescriptor == (void *)0))
  {
    /* Invalid incoming data */
    return (-1);
  }

  /* Open the gpio device */
  if ((GpioDescriptor = open("/dev/gpiochip0", 0)) < 0)
  {
    return (-1);
  }

  /* Set the MISO as input (PA0) */
  MisoRequest.lineoffsets[0] = 0;
  MisoRequest.lines = 1;
  MisoRequest.flags = GPIOHANDLE_REQUEST_INPUT;
  strcpy(MisoRequest.consumer_label, "SPI_MISO");
  if (ioctl(GpioDescriptor, GPIO_GET_LINEHANDLE_IOCTL, &MisoRequest) < 0)
  {
    return (-1);
  }

  /* Set the MOSI as output (PA1) */
  MosiRequest.lineoffsets[0] = 1;
  MosiRequest.lines = 1;
  MosiRequest.flags = GPIOHANDLE_REQUEST_OUTPUT;
  strcpy(MosiRequest.consumer_label, "SPI_MOSI");
  if (ioctl(GpioDescriptor, GPIO_GET_LINEHANDLE_IOCTL, &MosiRequest) < 0)
  {
    return (-1);
  }

  /* Set the default value on MOSI output (0) */
  MosiData.values[0] = 0;
  if (ioctl(MosiRequest.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &MosiData) < 0)
  {
    return (-1);
  }

  /* Set the SPCK as output (PA2) */
  SpckRequest.lineoffsets[0] = 2;
  SpckRequest.lines = 1;
  SpckRequest.flags = GPIOHANDLE_REQUEST_OUTPUT;
  strcpy(SpckRequest.consumer_label, "SPI_SPCK");
  if (ioctl(GpioDescriptor, GPIO_GET_LINEHANDLE_IOCTL, &SpckRequest) < 0)
  {
    return (-1);
  }

  /* Set the default value on SPCK output (0) */
  SpckData.values[0] = 0;
  if (ioctl(SpckRequest.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &SpckData) < 0)
  {
    return (-1);
  }

  /* Set the NPCS0 as output (PA3) */
  Npcs0Request.lineoffsets[0] = 3;
  Npcs0Request.lines = 1;
  Npcs0Request.flags = GPIOHANDLE_REQUEST_OUTPUT;
  strcpy(Npcs0Request.consumer_label, "SPI_NPCS0");
  if (ioctl(GpioDescriptor, GPIO_GET_LINEHANDLE_IOCTL, &Npcs0Request) < 0)
  {
    return (-1);
  }

  /* Set the default value on NPCS0 output (1) */
  Npcs0Data.values[0] = 1;
  if (ioctl(Npcs0Request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &Npcs0Data) < 0)
  {
    return (-1);
  }

  *SpiGpioDescriptor = GpioDescriptor;
  *SpiMisoDescriptor = MisoRequest.fd;
  *SpiMosiDescriptor = MosiRequest.fd;
  *SpiSpckDescriptor = SpckRequest.fd;
  *SpiNpcs0Descriptor = Npcs0Request.fd;

  /* Successfull exit */
  return 0;
}

int ActivateNpcs0(int SpiNpcs0Descriptor)
{
  struct gpiohandle_data Npcs0Data;

  Npcs0Data.values[0] = 0;
  if (ioctl(SpiNpcs0Descriptor, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &Npcs0Data) < 0)
  {
    return (-1);
  }
  return 0;
}

int DeactivateNpcs0(int SpiNpcs0Descriptor)
{
  struct gpiohandle_data Npcs0Data;

  Npcs0Data.values[0] = 1;
  if (ioctl(SpiNpcs0Descriptor, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &Npcs0Data) < 0)
  {
    return (-1);
  }
  return 0;
}

int SetSpck(int SpiSpckDescriptor)
{
  struct gpiohandle_data SpckData;

  SpckData.values[0] = 1;
  if (ioctl(SpiSpckDescriptor, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &SpckData) < 0)
  {
    return (-1);
  }
  return 0;
}

int ClearSpck(int SpiSpckDescriptor)
{
  struct gpiohandle_data SpckData;

  SpckData.values[0] = 0;
  if (ioctl(SpiSpckDescriptor, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &SpckData) < 0)
  {
    return (-1);
  }
  return 0;
}

int SetMosi(int SpiMosiDescriptor)
{
  struct gpiohandle_data MosiData;

  MosiData.values[0] = 1;
  if (ioctl(SpiMosiDescriptor, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &MosiData) < 0)
  {
    return (-1);
  }
  return 0;
}

int ClearMosi(int SpiMosiDescriptor)
{
  struct gpiohandle_data MosiData;

  MosiData.values[0] = 0;
  if (ioctl(SpiMosiDescriptor, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &MosiData) < 0)
  {
    return (-1);
  }
  return 0;
}

unsigned int ReadMiso(int SpiMisoDescriptor)
{
  struct gpiohandle_data MisoData;

  /* Get the MISO state */
  if (ioctl(SpiMisoDescriptor, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &MisoData) < 0)
  {
    return 0;
  }
  return (unsigned int)MisoData.values[0];
}

int WriteDataToSpiBus(int SpiSpckDescriptor, int SpiMosiDescriptor, unsigned char DataToWriteToSpiBus)
{
  unsigned char BitMask = 0x80;

  /* Initial state SPCK = 0, MOSI = 0 */
  if (ClearSpck(SpiSpckDescriptor) < 0)
  {
    return (-1);
  }

  if (ClearMosi(SpiMosiDescriptor) < 0)
  {
    return (-1);
  }

  do
  {
    /* Set or clear MOSI pin */
    if (DataToWriteToSpiBus & BitMask)
    {
      if (SetMosi(SpiMosiDescriptor) < 0)
      {
        return (-1);
      }
    }
    else
    {
      if (ClearMosi(SpiMosiDescriptor) < 0)
      {
        return (-1);
      }
    }

    /* Set SPCK = 1 */
    if (SetSpck(SpiSpckDescriptor) < 0)
    {
      return (-1);
    }

    /* Delay */
    if (SetSpck(SpiSpckDescriptor) < 0)
    {
      return (-1);
    }

    /* Clear SPCK = 0 */
    if (ClearSpck(SpiSpckDescriptor) < 0)
    {
      return (-1);
    }

    /* Go to the next bit in byte of data */
    BitMask >>= 1;
  } while (BitMask > 0);

  /* Final state SPCK = 0, MOSI = 0 */
  if (ClearSpck(SpiSpckDescriptor) < 0)
  {
    return (-1);
  }

  if (ClearMosi(SpiMosiDescriptor) < 0)
  {
    return (-1);
  }

  /* Normal exit */
  return 0;
}

int WriteAndReadDataFromSpiBus(int SpiSpckDescriptor, int SpiMosiDescriptor, int SpiMisoDescriptor,  unsigned char DataToWriteToSpiBus, unsigned char *ReadSpiBusData)
{
  unsigned short int BitMask = 0x100;

  /* Initial state SPCK = 0, MOSI = 0 */
  if (ClearSpck(SpiSpckDescriptor) < 0)
  {
    return (-1);
  }

  if (ClearMosi(SpiMosiDescriptor) < 0)
  {
    return (-1);
  }

  *ReadSpiBusData = 0x0;

  do
  {
    /* Set or clear MOSI pin, depending on data to write content */
    if (DataToWriteToSpiBus & ((unsigned char)((BitMask >> 1) & 0xFF)))
    {
      if (SetMosi(SpiMosiDescriptor) < 0)
      {
        return (-1);
      }
    }
    else
    {
      if (ClearMosi(SpiMosiDescriptor) < 0)
      {
        return (-1);
      }
    }

    /* Set SPCK = 1 */
    if (SetSpck(SpiSpckDescriptor) < 0)
    {
      return (-1);
    }

    /* Read the answer from MISO pin beginning from second bit */
    if (ReadMiso(SpiMisoDescriptor))
    {
      *ReadSpiBusData |= (unsigned char)(BitMask & 0xFF);
    }

    /* Clear SPCK = 0 */
    if (ClearSpck(SpiSpckDescriptor) < 0)
    {
      return (-1);
    }

    /* Go to the next bit in byte of data */
    BitMask >>= 1;
  } while (BitMask > 0);

  /* Final state SPCK = 0, MOSI = 0 */
  if (ClearSpck(SpiSpckDescriptor) < 0)
  {
    return (-1);
  }

  if (ClearMosi(SpiMosiDescriptor) < 0)
  {
    return (-1);
  }

  /* Normal exit */
  return 0;
}

int CloseSpiBus(int SpiGpioDescriptor)
{
  /* Check the incoming data */
  if (SpiGpioDescriptor < 0)
  {
    /* Invalid incoming data */
    return (-1);
  }

  close(SpiGpioDescriptor);

  /* Successfull exit */
  return 0;
}
