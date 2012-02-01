CC = arm-linux-gnueabi-gcc

all:
	$(CC) test_ion.c -o test_ion -I$(KDIR)
