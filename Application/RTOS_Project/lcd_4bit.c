/**
* @file LCD_Driver_4bit_MODE.c
* @brief 4 bit mode lcd driver for ATMEGA16 or 32 .  
* @author Islam Mohamed.
*/

//The driver is provided to us with fully implemented ready functions
//I just added lcd_disp_number function

#include"lcd_4bit.h"
#include "macros.h"
#include <avr/io.h>
#include <util/delay.h>

/////////////////////////////////////////////////////////////////////
//	lcd enable triggering function to make lcd aware about command or
//   data changes.
////////////////////////////////////////////////////////////////////
void enableTrigger(void)
{

	CLR_BIT(PORTB,2);
	
	_delay_ms(1);

	SET_BIT(PORTB,2);
	_delay_ms(6);


}
///////////////////////////////////////////////////////////////////////////////////////
// LCD initialization API : set portB from pinB.4 to pinB.7 as o/p for data and commands 
//							 and pinB.0 to pinB.2 o/p for control pins 
//////////////////////////////////////////////////////////////////////////////////////
void lcd_init(void)
{

	DDRB=0xff; // init port B as O/P port 
	PORTB=0x04; // activate LCD enable 

	lcd_sendCommand(0x33); // 4-bit mode 
	lcd_sendCommand (0x32);
	lcd_sendCommand (0x28);



	lcd_sendCommand(0x0c); // turn on lcd 

		
}


void lcd_sendCommand(unsigned char cmd)
{

	unsigned char high_nibble , low_nibble ; 		
	
	HIGH_NIBBLE(high_nibble,cmd);
	LOW_NIBBLE(low_nibble,cmd);

	CLR_BIT(PORTB,0); // to enable command mode 
	CLR_BIT(PORTB,1); // write signal to lcd 

	PORTB &=0x0f;
	PORTB |=high_nibble;// to send high nibble command  
	enableTrigger(); // triggre lcd enable 

	PORTB &=0x0f;
	PORTB |=low_nibble; // to send low nibble command 
	enableTrigger();

}

void lcd_displayChar (unsigned char data)
{

	unsigned char high_nibble , low_nibble ;

	SET_BIT(PORTB,0); //  enable data mode 
	CLR_BIT(PORTB,1); // activate lcd write 

	HIGH_NIBBLE(high_nibble,data); // get high nibble data 
	LOW_NIBBLE(low_nibble,data); // get low nibble data 
	
	PORTB &= 0x0f;
	PORTB |=high_nibble;// to send high nibble data  
	enableTrigger(); // triggre lcd enable 
	
	PORTB &=0x0f;
	PORTB |=low_nibble; // to send low nibble data 
	enableTrigger();
	

}


void lcd_gotoxy(unsigned char y , unsigned char x )
{
	unsigned char position = 0x80;
	 
	switch(y)
	{
		case 0:

			position=position+x;
					
		break;

		case 1:

			position=0xc0;
			position=position+x;

		break;

		case 2:

			position=position+x;

		break;

		case 3:
			position=position+x;
		break;
	
	
	}
	
	lcd_sendCommand(position); 	


}


void lcd_clrScreen(void)
{

	lcd_sendCommand(0x01);


}


void lcd_dispString(char * ptr)
{

	while(* ptr!= '\0')
	{
		lcd_displayChar(* ptr);
		ptr++;
	
	}



}

void lcd_disp_string_xy(char * ptr , int y , int x)
{

	lcd_gotoxy(y,x);
	lcd_dispString(ptr);

	


}

void lcd_disp_number(unsigned int number){
	char string[16] = {0};
	short int i = 0;
	if(number == 0){
		lcd_displayChar('0');
		return;
	}

	while(number > 0){
		string[i] = (number%10) + '0';
		number = number / 10;
		i++;
	}

	i--;
	while(i >= 0){
		lcd_displayChar(string[i]);
		i--;
	}
}

