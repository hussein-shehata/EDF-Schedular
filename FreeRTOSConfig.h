/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc21xx.h>
#include "GPIO.h"



/* Variables for Calculating the times and CPU Load*/
extern int button1_in_time, button1_out_time,button1_total_time;
extern int button2_in_time, button2_out_time,button2_total_time;
extern int periodic_transmitter_in_time, periodic_transmitter_out_time,periodic_transmitter_total_time;
extern int receiver_in_time, receiver_out_time,receiver_total_time;
extern int load1_in_time, load1_out_time,load1_total_time;
extern int load2_in_time, load2_out_time,load2_total_time;

extern int system_time;
extern int cpu_load;
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/               

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			1
#define configUSE_TICK_HOOK			1
#define configCPU_CLOCK_HZ			( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 90 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN  	( 8 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configUSE_TIME_SLICING           1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configUSE_MUTEXES                1
#define configUSE_APPLICATION_TASK_TAG   1

/*added by me */
#define configUSE_EDF_SCHEDULER   1


/* trace hooks definitions */

#define traceTASK_SWITCHED_OUT()	if ((int)pxCurrentTCB->pxTaskTag == 2)\
		{\
	        GPIO_write (PORT_0, PIN2, PIN_IS_LOW);\
					button1_out_time = T1TC;\
					button1_total_time += button1_out_time - button1_in_time;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 3)\
		{\
			GPIO_write (PORT_0, PIN3, PIN_IS_LOW);\
			button2_out_time = T1TC;\
			button2_total_time += button2_out_time - button2_in_time;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 4)\
		{\
			GPIO_write (PORT_0, PIN4, PIN_IS_LOW);\
			periodic_transmitter_out_time = T1TC;\
			periodic_transmitter_total_time += periodic_transmitter_out_time - periodic_transmitter_in_time;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 5)\
		{\
			GPIO_write (PORT_0, PIN5, PIN_IS_LOW);\
			receiver_out_time = T1TC;\
			receiver_total_time += receiver_out_time - receiver_in_time;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 6)\
		{\
			GPIO_write (PORT_0, PIN6, PIN_IS_LOW);\
			load1_out_time = T1TC;\
			load1_total_time += load1_out_time - load1_in_time;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 7)\
		{\
			GPIO_write (PORT_0, PIN7, PIN_IS_LOW);\
			load2_out_time = T1TC;\
			load2_total_time += load2_out_time - load2_in_time;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 8)\
		{\
			GPIO_write (PORT_1, PIN1, PIN_IS_LOW);\
		}\
		\
		system_time = T1TC;\
		cpu_load = (button1_total_time + button2_total_time + periodic_transmitter_total_time + receiver_total_time + load1_total_time + load2_total_time ) /(float) system_time * 100 ;\

#define traceTASK_SWITCHED_IN()	  if ((int)pxCurrentTCB->pxTaskTag == 2)\
		{\
	        GPIO_write (PORT_0, PIN2, PIN_IS_HIGH);\
					button1_in_time = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 3)\
		{\
			GPIO_write (PORT_0, PIN3, PIN_IS_HIGH);\
			button2_in_time = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 4)\
		{\
			GPIO_write (PORT_0, PIN4, PIN_IS_HIGH);\
			periodic_transmitter_in_time = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 5)\
		{\
			GPIO_write (PORT_0, PIN5, PIN_IS_HIGH);\
			receiver_in_time = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 6)\
		{\
			GPIO_write (PORT_0, PIN6, PIN_IS_HIGH);\
			load1_in_time = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 7)\
		{\
			GPIO_write (PORT_0, PIN7, PIN_IS_HIGH);\
			load2_in_time = T1TC;\
		}\
		\
		else if ((int)pxCurrentTCB->pxTaskTag == 8)\
		{\
			GPIO_write (PORT_1, PIN1, PIN_IS_HIGH);\
		}\




#define configQUEUE_REGISTRY_SIZE 0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		      0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		  1
#define INCLUDE_uxTaskPriorityGet	      1
#define INCLUDE_vTaskDelete				  1
#define INCLUDE_vTaskCleanUpResources	  0
#define INCLUDE_vTaskSuspend			  1
#define INCLUDE_vTaskDelayUntil		   	  1
#define INCLUDE_vTaskDelay				  1



#endif /* FREERTOS_CONFIG_H */