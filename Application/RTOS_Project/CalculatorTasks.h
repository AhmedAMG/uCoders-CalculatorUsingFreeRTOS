/*
 * CalculatorTasks.h
 *
 * Created: 3/28/2020 7:53:23 PM
 *  Author: DELL
 */ 

#ifndef CALCULATORTASKS_H_
#define CALCULATORTASKS_H_

void vApplicationIdleHook( void );
void WelcomePattern_Task(void * pvParameters);
void PromptUserPattern_Task(void * pvParameters);
void KEYPAD_Task(void * pvParameters);
void LCD_Task(void * pvParameters);
void LED_Task(void * pvParameters);
void SW_Task(void * pvParameters);
void CALC_Task(void * pvParameters);
char KEYPAD_Scan(void);

#define BIT_WELCOME	(1<<0)
#define BIT_PROMPT	(1<<1)
#define BIT_CALC	(1<<2)
#define BIT_LED		(1<<3)
#define BIT_START	(1<<4)

#define READ_SW_PIN		(PIND&(1<<0))
//#define SET_LED_PIN		(PORTD |= (1<<7))
//#define RESET_LED_PIN	(PORTD &= ~(1<<7))
//#define TOGGLE_LED_PIN	(PORTD ^= (1<<7))

#define ONE_SECOND		(1000)
#define PROMPT_ON_TIME	(500)
#define PROMPT_OFF_TIME	(250)
#define LESS_THAN_3		'c'


#define WELCOME_PRIORITY	(3)
#define PROMPTUSER_PRIORITY (2)
#define LED_PRIORITY	(1)
#define LCD_PRIORITY	(1)
#define KEYPAD_PRIORITY (1)
#define SW_PRIORITY		(1)
#define CALC_PRIORITY	(1)

#define FAST_TEST	(0)
#define WELCOME_CYCLES	(3)
#define PROMPT_SECONDS	(10*ONE_SECOND)

typedef enum{
	ON_C = 1,
	ZERO = 2,
	EQUAL = 3,
	ADD = 12,
	SUB = 13,
	MUL = 14,
	DIV = 15,
}SIGNS;

typedef enum{
	RISING_STAGE = 1,
	HIGH_STAGE = 2,
	FALLING_STAGE = 3,
	LOW_STAGE = 4
}STAGE;

typedef struct{
	char lastDigit;
	unsigned int display;
	unsigned int CurrentNumber;
	unsigned int Answer;
	unsigned char LastOperation;
}CALC_DISP;

#endif /* CALCULATORTASKS_H_ */