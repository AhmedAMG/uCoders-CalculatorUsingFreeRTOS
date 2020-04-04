/*
 * RTOS_Project.c
 *
 * Created: 3/28/2020 3:23:42 PM
 * Author : DELL
 */ 

//The application is fully tested and verified on 3/4/2020

#define F_CPU 8000000
#include "avr/io.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "CalculatorTasks.h"
#include "lcd_4bit.h"
#include "TIM.h"

xTaskHandle WelcomePattern_Handle;
xTaskHandle PromptUser_Handle;
xTaskHandle LED_Handle;
xTaskHandle LCD_Handle;
xTaskHandle KEYPAD_Handle;
xTaskHandle SW_Handle;
xTaskHandle CALC_Handle;

EventGroupHandle_t event_handle;
QueueHandle_t queue_handle;

static CALC_DISP calc_disp = {0,0,0,0,0};
void *display_struct = &calc_disp;

void gpio_init(void);

int main( void )
{
	gpio_init();
	lcd_init();
	TIM2_Init();
	TIM2_SetDutyCycle(0);
	
	event_handle = xEventGroupCreate();
	queue_handle = xQueueCreate(4,sizeof(char));
	
	#if FAST_TEST == 0
	xTaskCreate( WelcomePattern_Task, "Welcome", 100, NULL, WELCOME_PRIORITY, &WelcomePattern_Handle);
	xTaskCreate( PromptUserPattern_Task, "Prompt", 100, NULL, PROMPTUSER_PRIORITY, &PromptUser_Handle);
	#endif
	
	xTaskCreate( LED_Task, "LED", 120, NULL, LED_PRIORITY, &LED_Handle);
	xTaskCreate( LCD_Task, "LCD", 120, display_struct, LCD_PRIORITY, &LCD_Handle);
	xTaskCreate( KEYPAD_Task, "Keypad", 120, NULL, KEYPAD_PRIORITY, &KEYPAD_Handle);
	xTaskCreate( SW_Task, "Switch", 120, NULL, SW_PRIORITY, &SW_Handle);
	xTaskCreate( CALC_Task, "Calculator", 120, display_struct, CALC_PRIORITY, &CALC_Handle);
	
	/* Start the scheduler. */
	vTaskStartScheduler();
	
	return 0;
}

void gpio_init(void){
	//Idle hook indication
	DDRA = 0xFF; //PORTA:OUTPUT
	
	//KEYPAD
	DDRC = 0x0F; //4pins:INPUT(Read) and 4pins:OUTPUT(Write)
	PORTC = 0xFF; //Pull up resistors for inputs and Set outputs

	//SW and LED
	DDRD = (1<<7); //Switch:INPUT and LED:OUTPUT

}

