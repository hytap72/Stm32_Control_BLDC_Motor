/*
 * rheostat.c
 *
 *  Created on: Nov 8, 2025
 *      Author: Admin
 */

#include "main.h"
#include "rheostat.h"


/* ----------------VARIABLES---------------------*/

/*	---------------PRIVATE FUNCTIONS---------------*/

 /**
  * @brief Cấu hình chân PC0
  */
static void ADC_Config(void){
	volatile uint32_t* RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*RCC_AHB1ENR |= (0b1 << 2);

	volatile uint32_t* GPIOC_MODER = (uint32_t*)(GPIOC_BASE_ADDR + 0x00);
	*GPIOC_MODER &=~ (0b11 << 0*2);
	*GPIOC_MODER |= (0b11 << 0*2);


	volatile uint32_t* RCC_APB2ENR = (uint32_t*)(RCC_BASE_ADDR + 0x44);
	*RCC_APB2ENR |= (0b1 << 8); 									//Cấp xung cho ADC1

	volatile uint32_t* ADC1_JSQR = (uint32_t*)(ADC1_BASE_ADDR + 0x38);
	volatile uint32_t* ADC1_CR2 = (uint32_t*)(ADC1_BASE_ADDR + 0x08);
	volatile uint32_t* ADC1_SMPR1 = (uint32_t*)(ADC1_BASE_ADDR + 0x0C);

	*ADC1_JSQR &=~ (0b00 << 20);									//select 1 conversion
	*ADC1_JSQR |= (10 << 15); 										//chanel 10
	*ADC1_CR2 |= (0b1 << 0); 										//enable ADC

	*ADC1_SMPR1 |= (0b001 << 0);
}



/*---------------PUBLIC FUNCTIONS--------------=*/



/**
 * @brief Hàm khởi tạo ADC1
 */
void ADC_Init(void){
	ADC_Config();
}



/**
 * @brief Hàm trả giá trị ADC1 (biến trở)
 * @note Đọc thanh ghi ADC1_JDRx
 */
uint16_t ADC1_Get_Val(void){
	volatile uint32_t* ADC1_CR2 = (uint32_t*)(ADC1_BASE_ADDR + 0x08);
	volatile uint32_t* ADC1_SR = (uint32_t*)(ADC1_BASE_ADDR + 0x00);
	volatile uint32_t* ADC1_JDR1 = (uint32_t*)(ADC1_BASE_ADDR + 0x3C);

	*ADC1_CR2 &=~ (0b1 << 22);
	*ADC1_CR2 |= (0b1 << 22);
	while(!((*ADC1_SR >> 2) & 1));
	*ADC1_SR &=~ (0b1 << 3);
	return (uint16_t)(*ADC1_JDR1);
}
