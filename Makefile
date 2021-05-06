CC=arm-softfloat-linux-gnueabi-gcc
CFLAGS= -static -O0 -Wall -pedantic -std=c89

flasher:	main.o io.o at45db642.o spi.o
		$(CC) $(CFLAGS) -o flasher main.o io.o at45db642.o spi.o
		arm-softfloat-linux-gnueabi-strip --strip-unneeded flasher
main.o:		main.c
		$(CC) $(CFLAGS) -c main.c
io.o:		io.c io.h
		$(CC) $(CFLAGS) -c io.c
at45db642.o:	at45db642.c at45db642.h
		$(CC) $(CFLAGS) -c at45db642.c
spi.o:		spi.c spi.h
		$(CC) $(CFLAGS) -c spi.c
clean:
		rm -f *.o flasher
