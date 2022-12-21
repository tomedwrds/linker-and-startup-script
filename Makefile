CC=arm-none-eabi-gcc
CFLAGS= -c -mcpu=cortex-m4 -mthumb -Wall -std=gnu11
LDFLAGS = -nostdlib -T stm32_ls.ld -Wl,-Map=final.map

all: main.o led.o stm32_startup.o final.elf

main.o:main.c
	$(CC) $(CFLAGS) -o $@ $^


led.o:led.c
	$(CC) $(CFLAGS) -o $@ $^

stm32_startup.o:stm32_startup.c
	$(CC) $(CFLAGS) -o $@ $^

final.elf: main.o led.o stm32_startup.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	del -rf *.o