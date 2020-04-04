/*
 * TIM.c
 *
 *  Created on: Oct 11, 2019
 *      Author: DELL
 */

#include "avr/io.h"
#include "TIM.h"

void TIM2_Init(void){
	SetBit(TCCR2,3);
	SetBit(TCCR2,6);

	SetBit(TCCR2,5);

	if(((float)Clk_Freq/(Req_Freq*256.0)) < 8){
		TCCR2 = (TCCR2 & (~0x07)) | (1 << 0);
	}else if(((float)Clk_Freq/(Req_Freq*256.0)) < 32){
		TCCR2 = (TCCR2 & (~0x07)) | (2 << 0);
	}else if(((float)Clk_Freq/(Req_Freq*256.0)) < 64){
		TCCR2 = (TCCR2 & (~0x07)) | (3 << 0);
	}else if(((float)Clk_Freq/(Req_Freq*256.0)) < 128){
		TCCR2 = (TCCR2 & (~0x07)) | (4 << 0);
	}else if(((float)Clk_Freq/(Req_Freq*256.0)) < 256){
		TCCR2 = (TCCR2 & (~0x07)) | (5 << 0);
	}else if(((float)Clk_Freq/(Req_Freq*256.0)) < 1024){
		TCCR2 = (TCCR2 & (~0x07)) | (6 << 0);
	}else{
		TCCR2 = (TCCR2 & (~0x07)) | (7 << 0);
	}
}

void TIM2_SetDutyCycle(unsigned char DC){
	OCR2 = (unsigned char)((DC/100.0)*255);
}


