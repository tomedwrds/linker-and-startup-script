/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "led.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


//Function prototypes
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);
void init_systick_timer(uint32_t tick_h);
__attribute__((naked)) void init_scheduler_stack(uint32_t stack_start);
void init_task_stack(void);
void enable_proccessor_faults(void);
__attribute__((naked)) void switch_sp_to_psp(void);
void task_delay(uint32_t tick_count);
void unblock_tasks(void);
void update_global_tick_count(void);
void schedule(void);

//This struct contains all the data for each task
typedef struct
{
	uint32_t psp_value;
	uint32_t block_count;
	uint8_t current_state;
	void (*task_handler)(void);
}TCB_t;

TCB_t user_tasks[MAX_TASKS];

//Global variables that holds current task
uint8_t current_task = 1; //0 denotes task 1
uint32_t g_tick_count = 0; //0 denotes task 1

int main(void)
{
	//Enable proccessor faults
	enable_proccessor_faults();

	//Sets up the pointer to point to systic handler when progarmme begins running
	init_scheduler_stack(SCHEDULER_STACK_START);


	//Tasks mus have there stack intalized as when a task is ran is stack is loaded into core registers. Dummy values must be provided for its first time running
	init_task_stack();

	//Initlize led
	led_init_all();

	//Initalize the systick timer
	init_systick_timer(TICK_HZ);

	//setup has been done using msp pointer. The tasks run with psp
	switch_sp_to_psp();

	//Run the 1st task
	task1_handler();





    /* Loop forever */
	for(;;);
}


//This handler handles context swithcing between tasks
void SysTick_Handler(void)
{
	//Updates tick count and check if it allows a task to be unblocked
	update_global_tick_count();
	unblock_tasks();
	schedule();



}

//PendSV handles the context swaping in a low noise enviroment ie no other assembly code ran
__attribute__((naked)) void PendSV_Handler(void)
{
	//Save the context of current task
	//Get current tasks PSP
	__asm volatile("MRS R0,PSP");
	//Using value store current tasks context SF2 R4 to R11
	__asm volatile("STMDB R0!,{R4-R11}");

	//Save content of LR
	__asm volatile("PUSH {LR}");

	//Calls the save psp function R0 is first argument then returns
	__asm volatile("BL save_psp_value");

	//Set the next task
	__asm volatile("BL update_next_task");

	//Get the psp value it will be store in R0 then store it to psp
	__asm volatile("BL get_psp");


	//Retrieve the values
	__asm volatile("LDMIA R0!, {R4-R11}");

	//Save new PSP value
	__asm volatile("MSR PSP, R0");

	//Get the LR value back
	__asm volatile("POP {LR}");


	__asm volatile("BX LR");
}

void idle_task(void)
{
	while(1);
}

void task1_handler(void)
{
	//These tasks never return so infinite loop
	while(1)
	{
		led_on(LED_GREEN);
		task_delay(DELAY_COUNT_1S);
		led_off(LED_GREEN);
		task_delay(DELAY_COUNT_1S);
	}

}
void task2_handler(void)
{
	//These tasks never return so infinite loop
	while(1)
	{
		led_on(LED_ORANGE);
		task_delay(DELAY_COUNT_500MS);
		led_off(LED_ORANGE);
		task_delay(DELAY_COUNT_500MS);
	}
}
void task3_handler(void)
{
	//These tasks never return so infinite loop
	while(1)
	{
		led_on(LED_BLUE);
		task_delay(DELAY_COUNT_250MS);
		led_off(LED_BLUE);
		task_delay(DELAY_COUNT_250MS);
	}
}
void task4_handler(void)
{
	//These tasks never return so infinite loop
	while(1)
	{
		led_on(LED_RED);
		task_delay(DELAY_COUNT_125MS);
		led_off(LED_RED);
		task_delay(DELAY_COUNT_125MS);
	}

}

void unblock_tasks(void)
{
	//Loop through all tasks and check if they can be unblocked
	for(int i = 0; i < MAX_TASKS; i++)
	{
		if(user_tasks[i].current_state != TASK_RUNNING_STATE)
		{
			//Delay is complete is finished
			if(user_tasks[i].block_count == g_tick_count)
			{
				user_tasks[i].current_state = TASK_RUNNING_STATE;
			}
		}
	}
}


void init_systick_timer(uint32_t tick_h)
{
	//This is the required clock cycles for the desired TICK HZ
	//N-1 is used as the SYS_TICK_HANDLER is called 1 clock cycle after the value reaches 0
	uint32_t count = (SYSTICK_TIM_CLK/TICK_HZ)-1;

	//Clear and set the systick relaod value. The systic reload value determiens the amount of clock cycles between the systick handler being called
	uint32_t* pSYST_RVR = (uint32_t *) 0xE000E014;
	*pSYST_RVR &= ~(0x00FFFFFF);
	*pSYST_RVR |= count;

	//Setup and configure the systic register
	uint32_t* SYST_CSR = (uint32_t *) 0xE000E010;

	*SYST_CSR |= (1<<0); //Enables the systick
	*SYST_CSR |= (1<<1); //Enables the interupt being called when systic counter reaches 0
	*SYST_CSR |= (1<<2); //Set the systick source to the proccessor clock

}


//Naked function as we dont want prolog epilouge messing up the MSP value
//This function sets the value of MSP ot the systick handler then returns to main
__attribute__((naked)) void init_scheduler_stack(uint32_t stack_start)
{
	__asm volatile("MSR MSP, %0"::"r"(stack_start):);
	__asm volatile("BX LR");

}

void save_psp_value(uint32_t current_psp_value)
{
	user_tasks[current_task].psp_value = current_psp_value;
}

//This allows for task scheduler to be round robin by rotating around tasks
void update_next_task(void)
{
	int state = TASK_BLOCKED_STATE;

	for(int i =0; i < MAX_TASKS; i++)
	{
		//Increment current task and apply modulus
		current_task += 1;
		current_task %= MAX_TASKS;
		state = user_tasks[current_task].current_state;

		//Check if state is set to running state and not idle task
		if((state == TASK_RUNNING_STATE) && (current_task !=0))
			break;
	}

	//If no running tasks run the idle task
	if(state !=TASK_RUNNING_STATE)
		current_task = 0;

}

void update_global_tick_count(void)
{
	g_tick_count ++;
}

void init_task_stack(void)
{

	//Defien the structs for each task
	user_tasks[0].current_state = TASK_RUNNING_STATE;
	user_tasks[1].current_state = TASK_RUNNING_STATE;
	user_tasks[2].current_state = TASK_RUNNING_STATE;
	user_tasks[3].current_state = TASK_RUNNING_STATE;
	user_tasks[4].current_state = TASK_RUNNING_STATE;

	user_tasks[0].psp_value = IDLE_STACK_START;
	user_tasks[1].psp_value = T1_STACK_START;
	user_tasks[2].psp_value = T2_STACK_START;
	user_tasks[3].psp_value = T3_STACK_START;
	user_tasks[4].psp_value = T4_STACK_START;

	user_tasks[0].task_handler = idle_task;
	user_tasks[1].task_handler = task1_handler;
	user_tasks[2].task_handler = task2_handler;
	user_tasks[3].task_handler = task3_handler;
	user_tasks[4].task_handler = task4_handler;



	//This is changed to stores the psp value for each task
	uint32_t *pPSP;

	//Intalize all 4 tasks
	for(int i = 0; i < MAX_TASKS; i++)
	{
		//Get the pointer value
		pPSP = (uint32_t *) user_tasks[i].psp_value;


		//The order of SF is XPSR, PC, LR, R12, R3, R2, R1, R0, R11, R10, R9, R8, R7, R6, R5, R4
		//The stack is full descending so -- must be used to set the next value in stack
		pPSP --;

		//Dummy XPSR sets the tbit to 1
		*pPSP = DUMMY_XPSR;

		pPSP--;
		//PC contains the adress of the function be called so instructions run from there
		*pPSP = (uint32_t)user_tasks[i].task_handler;

		pPSP --;
		//LR must be set to this value so the code runs in thread mode with PSP pointer
		*pPSP = 0xFFFFFFFD;

		//All of the other registers can be set to 0
		for(int j =0; j < 13; j++)
		{
			pPSP --;
			*pPSP = 0;
		}

		//The value of PSP needs to be preserved such that when this task is run the PSP starts at the end of the stack
		user_tasks[i].psp_value = (uint32_t)pPSP;


	}
}

uint32_t get_psp(void)
{
	return(user_tasks[current_task].psp_value);
}


__attribute__((naked)) void switch_sp_to_psp(void)
{
	//Intalize the psp with task 1 stack start
	//BL used as want to return to this function. AACP states that r0 will be set to the return value
	//LR value get corrupted on branch so its nesscary to push and pop it to stack
	__asm volatile("PUSH {LR}");
	__asm volatile("BL get_psp");
	__asm volatile("MSR PSP, R0");
	__asm volatile("POP {LR}");

	//Change the control register from msp to psp
	__asm volatile("MOV R0, #0x02");
	__asm volatile("MSR CONTROL, R0");
	__asm volatile("BX LR");

}

void schedule(void)
{
	//Now pend the pendsv function
	uint32_t * pISCR = (uint32_t*)0xE000ED04;
	*pISCR |= (1<<28);
}


void task_delay(uint32_t tick_count)
{
	//As we are accessing global data we dont want an interupt to occur
	//disable interupt
	INTERRUPT_DISABLE();

	if(current_task)
	{
		//Block count stores at what global tick count value the task should no longer be blocked
		user_tasks[current_task].block_count = g_tick_count + tick_count;
		user_tasks[current_task].current_state = TASK_BLOCKED_STATE;
		schedule();
	}

	//enable interupt
	INTERRUPT_ENABLE();
}


void enable_proccessor_faults(void)
{
	//1. enable all configurable exceptions : usage fault, mem manage fault and bus fault

	uint32_t *pSHCSR = (uint32_t*)0xE000ED24;

	*pSHCSR |= ( 1 << 16); //mem manage
	*pSHCSR |= ( 1 << 17); //bus fault
	*pSHCSR |= ( 1 << 18); //usage fault
}

//All the fault handler
void HardFault_Handler(void)
{
	while(1);
}


void MemManage_Handler(void)
{

	while(1);
}

void BusFault_Handler(void)
{
	while(1);
}
