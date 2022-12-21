/*
 * main.h
 *
 *  Created on: 20/12/2022
 *      Author: tomed
 */

#ifndef MAIN_H_
#define MAIN_H_

//Stack memory calculations macros
//All tasks and scheduler are of size 1 byte
#define SIZE_TASK_STACK 		1024U
#define SIZE_SCHEDULER_STACK 	1024U
#define SRAM_START 				0x20000000U
#define SRAM_SIZE				((128)*(SIZE_TASK_STACK))
#define T1_STACK_START   		((SRAM_SIZE) + (SRAM_START))
#define T2_STACK_START  		((T1_STACK_START)-(SIZE_TASK_STACK))
#define T3_STACK_START  		((T2_STACK_START)-(SIZE_TASK_STACK))
#define T4_STACK_START 			((T3_STACK_START)-(SIZE_TASK_STACK))
#define SCHEDULER_STACK_START 	((T4_STACK_START)-(SIZE_TASK_STACK))
#define IDLE_STACK_START		((SCHEDULER_STACK_START)-(SIZE_TASK_STACK))

//This is how often we want to fire the systick handler. 1000HZ = 1ms. THe SYSTICK clock is at 16MHZ
#define TICK_HZ   				1000U
#define SYSTICK_TIM_CLK  		16000000U


//Task stack macros
#define MAX_TASKS 				5
#define DUMMY_XPSR				0x01000000
#define TASK_RUNNING_STATE 		0x00
#define TASK_BLOCKED_STATE 		0xFF


//SEtting the primask register prevents other excdptions from occuring
//Do while allows for the writing of multi line macros
#define INTERRUPT_DISABLE()  do{__asm volatile ("MOV R0,#0x1"); asm volatile("MSR PRIMASK,R0"); } while(0)
#define INTERRUPT_ENABLE()  do{__asm volatile ("MOV R0,#0x0"); asm volatile("MSR PRIMASK,R0"); } while(0)


#endif /* MAIN_H_ */
