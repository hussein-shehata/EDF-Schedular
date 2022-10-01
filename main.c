/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

/*****************************************Defintions used in the main **************************/

#define NULL_PTR 	(void *) 0
#define LOGIC_HIGH			1
#define LOGIC_LOW			0
#define BUTTON1_PORT		PORT_0
#define BUTTON1_PIN			PIN0

#define BUTTON2_PORT		PORT_0
#define BUTTON2_PIN			PIN1

/************** Periodicity of the Tasks************************/
#define BUTTON1_TASK_PERIOD				50
#define BUTTON2_TASK_PERIOD				50
#define PERIODIC_TASK_PERIOD			100
#define UART_TASK_PERIOD				20
#define LOAD1_TASK_PERIOD				10
#define LOAD2_TASK_PERIOD				100

/********************************************************************
**********************Task Handlers***********************************
************************************************************************/


TaskHandle_t Button_1_MonitorHandler = NULL;
TaskHandle_t Button_2_MonitorHandler = NULL;
TaskHandle_t Periodic_TransmitterHandler = NULL;
TaskHandle_t Uart_ReceiverHandler = NULL;

TaskHandle_t Load1Handler = NULL;
TaskHandle_t Load2Handler = NULL;

/******************************Queue Handler **************************/
QueueHandle_t xMessageBuffer = NULL;



/* Variables for Calculating the times and CPU Load*/
int button1_in_time, button1_out_time,button1_total_time = 0;
int button2_in_time, button2_out_time,button2_total_time = 0;
int periodic_transmitter_in_time, periodic_transmitter_out_time,periodic_transmitter_total_time = 0;
int receiver_in_time, receiver_out_time,receiver_total_time = 0;
int load1_in_time, load1_out_time,load1_total_time = 0;
int load2_in_time, load2_out_time,load2_total_time = 0;

int system_time = 0;
int cpu_load = 0;



/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/



/* Task to be created. */



void Button_1_Monitor(void * pvParameters)
{
	int xLastWakeTime = xTaskGetTickCount();
	pinState_t previous_state = PIN_IS_LOW , current_state = PIN_IS_LOW;  /* States for the Button input*/
	char* button1_message = NULL_PTR;	/* pointer to char holds the sent message */
	vTaskSetApplicationTaskTag(NULL,(void *) 2); /*giving Tag to the task to use Trace Hooks */
	
	for(;;)
	{
		current_state = GPIO_read(BUTTON1_PORT,BUTTON1_PIN); /* hold the current state of the button to compare it wuth the previous state*/
		if (previous_state != current_state )
		{
			if (current_state == PIN_IS_LOW)
			{
				button1_message = "Falling Edge @B1\n";
				xQueueSend(xMessageBuffer, (void *) &button1_message, ( TickType_t ) 0 ); /* sending the message to the queue */
			}
			else
			{
				button1_message = "Rising Edge @B1\n";
				xQueueSend(xMessageBuffer, (void *) &button1_message, ( TickType_t ) 0 ); /* sending the message to the queue */
			}
			
			previous_state = current_state;
		}
		vTaskDelayUntil(&xLastWakeTime,BUTTON1_TASK_PERIOD);
	}
}



void Button_2_Monitor(void * pvParameters)
{
	int xLastWakeTime = xTaskGetTickCount();
	pinState_t previous_state = PIN_IS_LOW , current_state = PIN_IS_LOW;  /* States for the Button input*/
	char* button2_message = NULL_PTR; /* pointer to char holds the sent message */
	vTaskSetApplicationTaskTag(NULL,(void *) 3); /*giving Tag to the task to use Trace Hooks */

	for(;;)
	{
		current_state = GPIO_read(BUTTON2_PORT,BUTTON2_PIN); /* hold the current state of the button to compare it wuth the previous state*/
		if (previous_state != current_state )
		{
			if (current_state == PIN_IS_LOW)
			{
				button2_message = "Falling Edge @B2\n";
				xQueueSend(xMessageBuffer, (void *) &button2_message, ( TickType_t ) 0 ); /* sending the message to the queue */
			}
			else
			{
				button2_message = "Rising Edge @B2\n";
				xQueueSend(xMessageBuffer, (void *) &button2_message, ( TickType_t ) 0 ); /* sending the message to the queue */
			}
			
			previous_state = current_state;
		}
		vTaskDelayUntil(&xLastWakeTime,BUTTON2_TASK_PERIOD);
	}
}





void Periodic_Transmitter( void * pvParameters)
{
	char* periodic_message = NULL_PTR; /* pointer to char holds the sent message */
	int xLastWakeTime = xTaskGetTickCount();

	vTaskSetApplicationTaskTag(NULL,(void *) 4); /*giving Tag to the task to use Trace Hooks */

	for(;;)
	{
		if ( ( uxQueueSpacesAvailable( xMessageBuffer ) ) > 0 && (xMessageBuffer != NULL) )
		{
			periodic_message = "Periodic_Message\n";
			xQueueSend(xMessageBuffer, (void *) &periodic_message, ( TickType_t ) 0 ); /* sending the message to the queue */
		}
		vTaskDelayUntil(&xLastWakeTime,PERIODIC_TASK_PERIOD);
	}

}


void Uart_Receiver( void * pvParameters)
{
	char* Received_message = NULL_PTR;
	int xLastWakeTime = xTaskGetTickCount();

	vTaskSetApplicationTaskTag(NULL,(void *) 5); /*giving Tag to the task to use Trace Hooks */

	for(;;)
	{
		if( xQueueReceive( xMessageBuffer, &( Received_message ), ( TickType_t ) 0 ) == pdPASS )
		{ 
			vSerialPutString(Received_message,20); /* receiving the message to the queue */
		}
		vTaskDelayUntil(&xLastWakeTime,UART_TASK_PERIOD);
	}
}






void Load_1_Simulation( void * pvParameters )
{
	int i;

	int xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL,(void *) 6);
    for( ;; )
    {
        /* Task code goes here. */
			
			for (i = 0; i < 37313; i++)
			{
				/* for loop to make the excutions time 5ms*/
			}
			
			vTaskDelayUntil(&xLastWakeTime,LOAD1_TASK_PERIOD);
    } 


}


void Load_2_Simulation( void * pvParameters )
{
	int i;

	int xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL,(void *) 7);
    for( ;; )
    {
        /* Task code goes here. */
			
			
			for ( i = 0; i < 89552; i++)
			{
				/* for loop to make the excutions time 12ms*/
			}
			
				
			
			
			vTaskDelayUntil(&xLastWakeTime,LOAD2_TASK_PERIOD);
    } 


}



/*Implement Tick and Idle Hooks */

void vApplicationTickHook(void)
{
	GPIO_write(PORT_1, PIN0, PIN_IS_HIGH);
	GPIO_write(PORT_1, PIN0, PIN_IS_LOW);
}


void vApplicationIdleHook(void)
{
	vTaskSetApplicationTaskTag(NULL,(void *) 8); /*giving Tag to the task to use Trace Hooks */
}



/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
    /* Create Tasks here */
	
	




	xTaskCreatePeriodic(
                    Button_1_Monitor,       /* Function that implements the task. */
                    "Button_1_Monitor",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Button_1_MonitorHandler, /* Used to pass out the created task's handle. */
										BUTTON1_TASK_PERIOD); /*Used t0 pass the period of the task*/


	xTaskCreatePeriodic(
                    Button_2_Monitor,       /* Function that implements the task. */
                    "Button_2_Monitor",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Button_2_MonitorHandler, /* Used to pass out the created task's handle. */
										BUTTON2_TASK_PERIOD); /*Used t0 pass the period of the task*/








	xTaskCreatePeriodic(
                    Periodic_Transmitter,       /* Function that implements the task. */
                    "Periodic_Transmitter",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Periodic_TransmitterHandler, /* Used to pass out the created task's handle. */
										PERIODIC_TASK_PERIOD);  /*Used t0 pass the period of the task*/    
										
										
										
	xTaskCreatePeriodic(
                    Uart_Receiver,       /* Function that implements the task. */
                    "Load_1_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Uart_ReceiverHandler, /* Used to pass out the created task's handle. */
										UART_TASK_PERIOD);     
	
	 xTaskCreatePeriodic(
                    Load_1_Simulation,       /* Function that implements the task. */
                    "Load_1_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Load1Handler, /* Used to pass out the created task's handle. */
										LOAD1_TASK_PERIOD);     /*Used t0 pass the period of the task*/
										
		xTaskCreatePeriodic(
                    Load_2_Simulation,       /* Function that implements the task. */
                    "Load_2_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Load2Handler, /* Used to pass out the created task's handle. */
										LOAD2_TASK_PERIOD); /*Used t0 pass the period of the task*/
																			
										
										
										
		/*Creating the Queue */						
		xMessageBuffer = xQueueCreate( 3, sizeof( unsigned char[15] ) );

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


