CC=arm-none-eabi-gcc
CFLAGS= -c -mcpu=cortex-m4 -mthumb -Wall -std=gnu11

all: main.o led.o stm32_startup.o

main.o:main.c
	$(CC) $(CFLAGS) -o $@ $^


led.o:led.c
	$(CC) $(CFLAGS) -o $@ $^

stm32_startup.o:stm32_startup.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	del -rf *.o