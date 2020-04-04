/*
 * CalculatorTasks.c
 *
 * Created: 3/28/2020 7:53:05 PM
 *  Author: DELL
 */ 

#define F_CPU 8000000
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "lcd_4bit.h"
#include "queue.h"
#include "semphr.h"

#include "CalculatorTasks.h"
#include "TIM.h"
#include "util/delay.h"

extern xTaskHandle PromptUser_Handle;
extern xTaskHandle LED_Handle;
extern xTaskHandle LCD_Handle;
extern xTaskHandle KEYPAD_Handle;
extern xTaskHandle SW_Handle;
extern xTaskHandle CALC_Handle;

extern EventGroupHandle_t event_handle;
extern QueueHandle_t queue_handle;

extern void *display_struct;

void vApplicationIdleHook( void ){
	PORTA ^= 0xFF;
	_delay_ms(500);
}

void WelcomePattern_Task(void * pvParameters){
	char delay = 55;
	char y = 9, flag = 1, i = 0;
	taskENABLE_INTERRUPTS();
	TickType_t tick = xTaskGetTickCount();
	while(1){
		lcd_disp_string_xy("       ",0,y);
		if(y == 9){
			flag = 1;
			delay = 56;
			if(i++ == WELCOME_CYCLES) break;
		}
		else if(y == 0){
			flag = 0;
			delay = 55;
		}
		
		if(flag == 0) y++;
		else y--;
		
		lcd_disp_string_xy("Welcome",0,y);
		
		tick = xTaskGetTickCount();
		vTaskDelayUntil(&tick,delay);
	}
	xEventGroupClearBits(event_handle,portMAX_DELAY);
	xEventGroupSetBits(event_handle,BIT_WELCOME);
	vTaskDelete(NULL);
}

void PromptUserPattern_Task(void * pvParameters){
	xEventGroupWaitBits(event_handle,BIT_WELCOME,pdFALSE,pdFALSE,portMAX_DELAY);
	unsigned char i = 0;
	TickType_t tick = xTaskGetTickCount();
	TickType_t start_tick = xTaskGetTickCount();
	while(1){		
		if(i == 0){
			if(xEventGroupGetBits(event_handle) & BIT_START){
				i = 2;
				break;
			}
			lcd_disp_string_xy("Press any key",0,1);
			lcd_disp_string_xy("to continue",1,2);
			i = 1;
			if(tick - start_tick + PROMPT_ON_TIME < PROMPT_SECONDS){
				vTaskDelayUntil(&tick,PROMPT_ON_TIME);	
			}else{
				vTaskDelayUntil(&tick,PROMPT_SECONDS-(tick-start_tick));
				break;
			}
		}else{
			if(xEventGroupGetBits(event_handle) & BIT_START){
				i = 2;
				break;
			}
			lcd_clrScreen();
			i = 0;
			if(tick - start_tick + PROMPT_OFF_TIME < PROMPT_SECONDS){
				vTaskDelayUntil(&tick,PROMPT_OFF_TIME);
			}else{
				vTaskDelayUntil(&tick,PROMPT_SECONDS-(tick-start_tick));
				break;
			}
		}
	}
	lcd_clrScreen();
	if(i != 2){
		xEventGroupSetBits(event_handle,BIT_LED);
	}
	xEventGroupSetBits(event_handle,BIT_PROMPT);
	vTaskDelete(NULL);
}

void KEYPAD_Task(void * pvParameters){
	#if FAST_TEST == 0
	xEventGroupWaitBits(event_handle,BIT_WELCOME,pdFALSE,pdFALSE,portMAX_DELAY);
	#endif
	char receive = 0;
	TickType_t tick = xTaskGetTickCount();
	while(1){
		receive = KEYPAD_Scan();
		if(receive != 0){
			xEventGroupClearBits(event_handle,BIT_LED);
			xEventGroupSetBits(event_handle,BIT_START);
			xQueueSend(queue_handle,&receive,100);
			
		}
		vTaskDelayUntil(&tick,20);
	}
	
}

void LCD_Task(void * pvParameters){
	#if FAST_TEST == 0
	xEventGroupWaitBits(event_handle,BIT_WELCOME,pdFALSE,pdFALSE,portMAX_DELAY);
	xEventGroupWaitBits(event_handle,BIT_PROMPT,pdFALSE,pdFALSE,portMAX_DELAY);
	#endif
	CALC_DISP* struct_ptr = pvParameters;
	char newNum = 0;
	while(1){
		xEventGroupWaitBits(event_handle,BIT_CALC,pdTRUE,pdFALSE,portMAX_DELAY);
		if(struct_ptr->lastDigit != 0){
			if(newNum == 1 || (struct_ptr->CurrentNumber == 0)){
				lcd_clrScreen();
			}
			lcd_displayChar(struct_ptr->lastDigit);
			struct_ptr->lastDigit = 0;
			newNum = 0;
		}else{
			lcd_clrScreen();
			lcd_disp_number(struct_ptr->display);
			newNum = 1;
		}
	}
}

void LED_Task(void * pvParameters){
	#if FAST_TEST == 0
	xEventGroupWaitBits(event_handle,BIT_WELCOME,pdFALSE,pdFALSE,portMAX_DELAY);
	xEventGroupWaitBits(event_handle,BIT_PROMPT,pdFALSE,pdFALSE,portMAX_DELAY);
	#endif
	STAGE stage = RISING_STAGE;
	char duty_cycle = 0;
	TickType_t tick = xTaskGetTickCount();
	while(1){
		xEventGroupWaitBits(event_handle,BIT_LED,pdFALSE,pdFALSE,portMAX_DELAY);
		switch(stage){
		case RISING_STAGE:
			duty_cycle += 10;
			TIM2_SetDutyCycle(duty_cycle);
			if(duty_cycle == 100){
				stage = HIGH_STAGE;
			}
			vTaskDelayUntil(&tick,25);
			break;
		case HIGH_STAGE:
			vTaskDelayUntil(&tick,250);
			stage = FALLING_STAGE;
			break;
		case FALLING_STAGE:
			duty_cycle -= 10;
			TIM2_SetDutyCycle(duty_cycle);
			if(duty_cycle == 0){
				stage = LOW_STAGE;
			}
			vTaskDelayUntil(&tick,25);
			break;
		case LOW_STAGE:
			vTaskDelayUntil(&tick,250);
			stage = RISING_STAGE;
			break;	
		}
	}
}

void SW_Task(void * pvParameters){
	#if FAST_TEST == 0
	xEventGroupWaitBits(event_handle,BIT_WELCOME,pdFALSE,pdFALSE,portMAX_DELAY);
	xEventGroupWaitBits(event_handle,BIT_PROMPT,pdFALSE,pdFALSE,portMAX_DELAY);
	#endif
	char new = 0, old = 0, less = LESS_THAN_3;
	TickType_t tick = xTaskGetTickCount();
	TickType_t begin = xTaskGetTickCount();
	TickType_t count = 0;
	while(1){
		new = (READ_SW_PIN);
		if(new == 0){ //Button Pressed
			if(old != new){
				old = new;
				begin = xTaskGetTickCount();
			}else{
				count = xTaskGetTickCount() - begin;
			}
		}else{ //Button Released
			old = 1;
			new = 1;
			count = 0;
		}
		if(new == 0){
			if(count < 3000){
				xQueueSend(queue_handle,&less,10);
			}else if(count > 3000){
				lcd_clrScreen(); //BIG VIOLATION
				xEventGroupSetBits(event_handle,BIT_LED);
			}
		}
		vTaskDelayUntil(&tick,20);
	}
	
}

void CALC_Task(void * pvParameters){
	#if FAST_TEST == 0
	xEventGroupWaitBits(event_handle,BIT_WELCOME,pdFALSE,pdFALSE,portMAX_DELAY);
	xEventGroupWaitBits(event_handle,BIT_PROMPT,pdFALSE,pdFALSE,portMAX_DELAY);
	#endif
	char receive = 0;
	char last_receive = 0;
	char newCalculation = 0;
	CALC_DISP* struct_ptr = pvParameters;
	struct_ptr->lastDigit = 0;
	struct_ptr->LastOperation = ADD;
	struct_ptr->CurrentNumber = 0;
	struct_ptr->Answer = 0;
	struct_ptr->display = 0;
	
	while(1){
		xQueueReceive(queue_handle,&receive,portMAX_DELAY);
		if((last_receive >= ADD && last_receive <= DIV) && (receive >= ADD && receive <= DIV)){
			struct_ptr->LastOperation = receive;
			last_receive = receive;
		}
		else if(receive != LESS_THAN_3){ //Keypad
			switch(receive){
			case ADD:
				switch(struct_ptr->LastOperation){
				case ADD:
					struct_ptr->Answer += struct_ptr->CurrentNumber;
					break;	
				case SUB:
					struct_ptr->Answer -= struct_ptr->CurrentNumber;
					break;
				case MUL:
					struct_ptr->Answer *= struct_ptr->CurrentNumber;
					break;
				case DIV:
					struct_ptr->Answer /= struct_ptr->CurrentNumber;
					break;				
				}
				struct_ptr->CurrentNumber = 0;
				struct_ptr->LastOperation = ADD;
				struct_ptr->display = struct_ptr->Answer;
				last_receive = receive;
				break;
			case SUB:
				switch(struct_ptr->LastOperation){
				case ADD:
					struct_ptr->Answer += struct_ptr->CurrentNumber;
					break;
				case SUB:
					struct_ptr->Answer -= struct_ptr->CurrentNumber;
					break;
				case MUL:
					struct_ptr->Answer *= struct_ptr->CurrentNumber;
					break;
				case DIV:
					struct_ptr->Answer /= struct_ptr->CurrentNumber;
					break;
				}
				struct_ptr->CurrentNumber = 0;
				struct_ptr->LastOperation = SUB;
				struct_ptr->display = struct_ptr->Answer;
				last_receive = receive;
				break;
			case MUL:
				switch(struct_ptr->LastOperation){
				case ADD:
					struct_ptr->Answer += struct_ptr->CurrentNumber;
					break;
				case SUB:
					struct_ptr->Answer -= struct_ptr->CurrentNumber;
					break;
				case MUL:
					struct_ptr->Answer *= struct_ptr->CurrentNumber;
					break;
				case DIV:
					struct_ptr->Answer /= struct_ptr->CurrentNumber;
					break;
				}
				struct_ptr->CurrentNumber = 0;
				struct_ptr->LastOperation = MUL;
				struct_ptr->display = struct_ptr->Answer;
				last_receive = receive;
				break;
			case DIV:
				switch(struct_ptr->LastOperation){
					case ADD:
				struct_ptr->Answer += struct_ptr->CurrentNumber;
					break;
					case SUB:
				struct_ptr->Answer -= struct_ptr->CurrentNumber;
					break;
					case MUL:
				struct_ptr->Answer *= struct_ptr->CurrentNumber;
					break;
					case DIV:
				struct_ptr->Answer /= struct_ptr->CurrentNumber;
					break;
				}
				struct_ptr->CurrentNumber = 0;
				struct_ptr->LastOperation = DIV;
				struct_ptr->display = struct_ptr->Answer;
				last_receive = receive;
				break;
			case ON_C:
				struct_ptr->CurrentNumber = 0;
				struct_ptr->display = 0;
				newCalculation = 1;
				last_receive = 0;
				break;
				
			case EQUAL:
				switch(struct_ptr->LastOperation){
				case ADD:
					struct_ptr->Answer += struct_ptr->CurrentNumber;
					break;
				case SUB:
					struct_ptr->Answer -= struct_ptr->CurrentNumber;
					break;
				case MUL:
					struct_ptr->Answer *= struct_ptr->CurrentNumber;
					break;
				case DIV:
					struct_ptr->Answer /= struct_ptr->CurrentNumber;
					break;
				}
				struct_ptr->CurrentNumber = 0;
				struct_ptr->LastOperation = EQUAL;
				struct_ptr->display = struct_ptr->Answer;
				last_receive = receive;
				break;
			default:
				if(receive == ZERO) receive = '0';
				if(struct_ptr->LastOperation == EQUAL) newCalculation = 1;
				struct_ptr->CurrentNumber = struct_ptr->CurrentNumber * 10 + (receive-'0');
				struct_ptr->display = struct_ptr->CurrentNumber;
				struct_ptr->lastDigit = receive;
				last_receive = 0;
				break;
			}
			if(newCalculation == 1){
				struct_ptr->LastOperation = ADD;
				struct_ptr->Answer = 0;
				newCalculation = 0;	
			}
			xEventGroupSetBits(event_handle,BIT_CALC);
			
		}else{ //Switch
			if(last_receive != receive){
				struct_ptr->CurrentNumber = 0;
				struct_ptr->display = 0;
				struct_ptr->lastDigit = '0';
				last_receive = receive;
				xEventGroupSetBits(event_handle,BIT_CALC);
			}
		}
		
		xEventGroupSetBits(event_handle,BIT_CALC);
	}
}

//Warning: This can not handle 2 buttons pressed at the same time
char KEYPAD_Scan(void){
	char i = 0, j = 0, flag = 0, new = 0;
	static char old = 0;
	
	for(i = 0; i < 4; i++){
		PORTC |= 0x0F;
		PORTC &= ~(1<<i);
		for(j = 0; j < 4; j++){
			if( (( ~PINC & (1<<(j+4)) ) == (1<<(j+4))) && j != 0 && i != 3){ //For buttons : 1->9
				new = (j-1)*3 + i + 49;
				flag = 1;
				break;
			}else if( (( ~PINC & (1<<(j+4)) ) == (1<<(j+4))) && i == 3){ //For Buttons: Operations(ADD,SUB,MUL,DIV)
				new = j + ADD;
				flag = 1;
				break;
			}else if( (( ~PINC & (1<<4) ) == (1<<4)) ){ //For buttons: ON_C,0,EQUAL
				new = i+1;
				flag = 1;
				if(new == 2) new = '0';
				break;
			}
			
		}
		if(flag == 1) break;
	}
	
	if(flag == 1){ //Button Pressed
		if(old != new){
			old = new;
		}else{
			new = 0;
		}
	}else{ //Button Released
		old = 0;
		new = 0;
	}
	
	return new;
}



