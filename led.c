#include<stdint.h>
#include "led.h"



void delay(uint32_t count)
{
  for(uint32_t i = 0 ; i < count ; i++);
}

void led_init_all(void)
{

	//Enables the clock over the GPIOD peripheal
	uint32_t *pRccAhb1enr = (uint32_t*)0x40023830;
	*pRccAhb1enr |= ( 1 << 3);

	uint32_t *pGpiodModeReg = (uint32_t*)0x40020C00;

	//Set the mode reigester to output for all LED
	*pGpiodModeReg |= ( 1 << (2 * LED_GREEN));
	*pGpiodModeReg |= ( 1 << (2 * LED_ORANGE));
	*pGpiodModeReg |= ( 1 << (2 * LED_RED));
	*pGpiodModeReg |= ( 1 << (2 * LED_BLUE));

	//Intalize all LEDS as off
    led_off(LED_GREEN);
    led_off(LED_ORANGE);
    led_off(LED_RED);
    led_off(LED_BLUE);



}

void led_on(uint8_t led_no)
{
	//Set the GPIOD output register to 1 for selected bit. This turns on the LED
  uint32_t *pGpiodDataReg = (uint32_t*)0x40020C14;
  *pGpiodDataReg |= ( 1 << led_no);

}

void led_off(uint8_t led_no)
{
	//Set the GPIOD output register to 0 for selected bit. This turns off the LED
  uint32_t *pGpiodDataReg = (uint32_t*)0x40020C14;
  *pGpiodDataReg &= ~( 1 << led_no);

}
