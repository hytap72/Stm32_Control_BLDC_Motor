/*
 * motor_speed.c
 *
 *  Created on: Nov 9, 2025
 *      Author: Admin
 */

#include "main.h"
#include "motor_speed.h"

#define gear_ratio 4.4
/* ----------------VARIABLES---------------------*/
volatile uint32_t encoder_value = 0;
volatile uint32_t revolutions = 0, last_revolutions = 0;
volatile float speed, out_speed;
float max_speed = 0;

/*	---------------PRIVATE FUNCTIONS---------------*/

/**
 * @brief Cấu hình TIM2 ở chế độ đọc encoder
 * 		  Với 2 kênh A và B của encoder lần lượt ứng với TI1 - PA0 (TIM2_CH1) và TI2 - PA1 (TIM2_CH2)
 */
static void TIM2_Encoder_Config(void){
	uint32_t* RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*RCC_AHB1ENR |= (0b1 << 0);

	uint32_t* GPIOA_MODER =(uint32_t*)(GPIOA_BASE_ADDR + 0x00);
	uint32_t* GPIOA_AFRL = (uint32_t*)(GPIOA_BASE_ADDR + 0x20);
	uint32_t* GPIOA_PUPDR = (uint32_t*)(GPIOA_BASE_ADDR + 0x0C);

	*GPIOA_MODER &=~ (0b1111 << 0*2);
	*GPIOA_MODER |= (0b1010 << 0*2);										//Alternate function mode

	*GPIOA_AFRL &=~ ((0b1111 << 0) | (0b1111 << 4));
	*GPIOA_AFRL |= (0b0001 << 0) | (0b0001 << 4);							//AF01

	*GPIOA_PUPDR &=~ ((0b11 << 0*2) | (0b11 << 1*2));
	*GPIOA_PUPDR |= (0b01 << 0*2) | (0b01 << 1*2);							//pull up mode

	uint32_t* RCC_APB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x40);
	*RCC_APB1ENR |= (0b1 << 0);

	uint32_t* TIM2_SMCR = (uint32_t*)(TIM2_BASE_ADDR + 0x08);
	uint32_t* TIM2_CCMR1 = (uint32_t*)(TIM2_BASE_ADDR + 0x18);
	uint32_t* TIM2_CCER = (uint32_t*)(TIM2_BASE_ADDR + 0x20);
	uint32_t* TIM2_CR1 = (uint32_t*)(TIM2_BASE_ADDR + 0x00);
	uint32_t* TIM2_ARR = (uint32_t*)(TIM2_BASE_ADDR + 0x2C);
	uint32_t* TIM2_DIER = (uint32_t*)(TIM2_BASE_ADDR + 0x0C);
	uint32_t* NVIC_ISER0 = (uint32_t*)(NVIC_ISER0_BASE_ADDR + 0x00);

	*TIM2_CCMR1 &=~ ((0b11 << 0) | (0b11 << 8) | (0b1111 << 4) | (0b1111 << 10));
	*TIM2_CCMR1 |= (0b01 << 0) | (0b01 << 8);

	*TIM2_CCER &=~ ((0b1 << 1) | (0b1 << 3) | (0b1 << 5) | (0b1 << 7));

	*TIM2_ARR = 44;

	*TIM2_SMCR &=~ (0b111 << 0);
	*TIM2_SMCR |= (0b011 << 0);

	*TIM2_DIER |= (0b1 << 0);								//enable update interrupt
	*NVIC_ISER0 |= (0b1 << 28);

	*TIM2_CR1 |= (0b1 << 0);
}


static void TIM4_Motor_Speed_Config(void){
	uint32_t* RCC_APB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x40);
	*RCC_APB1ENR |= (0b1 << 2);

	uint32_t* TIM4_CR1 = (uint32_t*)(TIM4_BASE_ADDR + 0x00);
	uint32_t* TIM4_PSC = (uint32_t*)(TIM4_BASE_ADDR + 0x28);
	uint32_t* TIM4_ARR = (uint32_t*)(TIM4_BASE_ADDR + 0x2C);
	uint32_t* TIM4_DIER = (uint32_t*)(TIM4_BASE_ADDR + 0x0C);

	*TIM4_PSC = 16000 - 1;
	*TIM4_ARR = 1000;
	*TIM4_DIER |= (1 << 0); //enable interrupt

	uint32_t* NVIC_ISER0 = (uint32_t*)NVIC_ISER0_BASE_ADDR;
	*NVIC_ISER0 |= (0b1 << 30);

	*TIM4_CR1 |= (1 << 0); //counter enable
}



void TIM4_IRQHandler(){
	uint32_t* TIM4_SR = (uint32_t*)(TIM4_BASE_ADDR + 0x10);

	*TIM4_SR &= ~(1 << 0); // xóa cờ UIF
	uint32_t diff = 0;
	if(revolutions >= last_revolutions){
		diff = revolutions - last_revolutions;
	}
	else{
		diff = (0xFFFF - last_revolutions) + revolutions;
	}
	speed = (diff/1.0)*60.0;
	out_speed = speed/gear_ratio;
	last_revolutions = revolutions;

}



void TIM2_IRQHandler(){
	uint32_t* TIM2_SR = (uint32_t*)(TIM2_BASE_ADDR + 0x10);
	*TIM2_SR &= ~(1 << 0); // xóa cờ UIF
	revolutions++;
}

/*---------------PUBLIC FUNCTIONS---------------*/

void TIM2_Encoder_Init(void){
	TIM2_Encoder_Config();
}


void TIM4_Motor_Speed_Init(void){
	TIM4_Motor_Speed_Config();
}



uint32_t Get_Val_Encoder(){
	uint32_t* TIM2_CNT = (uint32_t*)(TIM2_BASE_ADDR + 0x24);
	return (uint32_t)(*TIM2_CNT);
}
