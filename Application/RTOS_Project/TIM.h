/*
 * TIM.h
 *
 *  Created on: Oct 11, 2019
 *      Author: DELL
 */

#ifndef TIM_H_
#define TIM_H_

#define Req_Freq 1000
#define Clk_Freq 8000000

#define SetBit(X,BIT) (X |= (1<<BIT))
#define ClearBit(X,BIT) (X &= ~(1<<BIT))
#define ToggleBit(X,BIT) (X ^= (1<<BIT))
#define GetBit(X,BIT) ((X & (1<<BIT)) >> BIT)

void TIM2_Init(void);
void TIM2_SetDutyCycle(unsigned char DC);

#endif /* TIM_H_ */
