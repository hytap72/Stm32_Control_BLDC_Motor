/*
 * motor.c
 *
 *  Created on: Nov 8, 2025
 *      Author: Admin
 */

#include "motor_ctrl.h"
#include "main.h"
#include "rheostat.h"

/* ----------------VARIABLES---------------------*/

uint8_t rever = 0; /* Cờ lưu chiều quay của động cơ*/
uint16_t val,\
	     duty_cycle;
float voltage ,\
	  max_motor_speed = 1360;

/*	---------------PRIVATE FUNCTIONS---------------*/


/*
 * Cấu hình nút nhần PB0 thành ngắt ngoài làm tín hiệu đảo chiều quay động cơ
*/
static void Motor_Direc_Signal_Config(void){
	volatile uint32_t* RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*RCC_AHB1ENR &=~ (0b1 << 1);
	*RCC_AHB1ENR |= (0b1 << 1);

	volatile uint32_t* GPIOB_MODER = (uint32_t*)(GPIOB_BASE_ADDR + 0x00);
	volatile uint32_t* GPIOB_PUPDR = (uint32_t*)(GPIOB_BASE_ADDR + 0x0C);
	*GPIOB_MODER &=~ (0b11 << 0*2);
	*GPIOB_PUPDR &=~ (0b11 << 0*2);
	*GPIOB_PUPDR |= (0b10 << 0*2);

	volatile uint32_t* RCC_APB2ENR = (uint32_t*)(RCC_BASE_ADDR + 0x44);
	*RCC_APB2ENR |= (0b1 << 14);

	volatile uint32_t* SYSCFG_EXTICR1 = (uint32_t*)(SYSCFG_BASE_ADDR + 0x08);
	*SYSCFG_EXTICR1 &=~ (0b1111 << 0);
	*SYSCFG_EXTICR1 |= (0b0001 << 0);

	volatile uint32_t* EXTI_IMR = (uint32_t*)(EXTI_BASE_ADDR + 0x00);
	volatile uint32_t* EXTI_RTRS = (uint32_t*)(EXTI_BASE_ADDR + 0x08);
	volatile uint32_t* ISER0 = (uint32_t*)(ISER_BASE_ADDR + 0x00);  // EXTI0 position 6 --> ISER0

	*EXTI_IMR &=~ (0b1 << 0);
	*EXTI_IMR |= (0b1 << 0); //Bật interrupt

	*EXTI_RTRS &=~ (0b1 << 0);
	*EXTI_RTRS |= (0b1 << 0);

	*ISER0 |= (0b1 << 6);
}

/*
 * Cấu hình PB8 PB9 làm output điều khiển In1 In2 trên module L295N đảo chiều động cơ
 */
static void Motor_Rota_Direction_Ctrl_Config(void){
	volatile uint32_t* RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);

	*RCC_AHB1ENR |= (0b1 << 1);

	volatile uint32_t* GPIOB_MODER = (uint32_t*)(GPIOB_BASE_ADDR + 0x00);

	*GPIOB_MODER &=~ ((0b11 << 8*2) | (0b11 << 9*2));
	*GPIOB_MODER |= (0b01 << 8*2) | (0b01 << 9*2);
}


/*
 * Hàm ngắt EXTIO0 đọc tín hiệu ngắt từ chân PB0
 * Mỗi lần có tín hiệu ngắt biến rever thay đổi giá trị thành 0 và 1
 * 	rever == 0 --> CLK
 * 	rever == 1 --> UCLK
 */
void EXTI0_IRQHandler(void){
	rever =! rever;
	volatile uint32_t* EXTI_PR = (uint32_t*)(EXTI_BASE_ADDR + 0x14);
	*EXTI_PR |= (0b1 << 0);
}


/**
 * @brief Hàm cấu hình chân PE9(TIM1_CH1) thành PWM
 */
void TIM1_PWM_Config(void){
	uint32_t* RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*RCC_AHB1ENR |= (0b1 << 4);
	uint32_t* GPIOE_MODER = (uint32_t*)(GPIOE_BASE_ADDR + 0x00);
	uint32_t* GPIOE_AFRH = (uint32_t*)(GPIOE_BASE_ADDR + 0x24);

	*GPIOE_MODER &=~ (0b11 << 9*2);
	*GPIOE_MODER |= (0b10 << 9*2);

	*GPIOE_AFRH &=~ (0b1111 << 4);
	*GPIOE_AFRH |= (0b0001 << 4);

	uint32_t* RCC_APB2ENR = (uint32_t*)(RCC_BASE_ADDR + 0x44);
	*RCC_APB2ENR |= (0b1 << 0);
	uint32_t* TIM1_PSC = (uint32_t*)(TIM1_BASE_ADDR + 0x28);
	uint32_t* TIM1_ARR = (uint32_t*)(TIM1_BASE_ADDR + 0x2C);
	uint32_t* TIM1_CCMR1 = (uint32_t*)(TIM1_BASE_ADDR + 0x18);
	uint32_t* TIM1_CCER = (uint32_t*)(TIM1_BASE_ADDR + 0x20);
	uint32_t* TIM1_BDTR = (uint32_t*)(TIM1_BASE_ADDR + 0x44);
	uint32_t* TIM1_CR1 = (uint32_t*)(TIM1_BASE_ADDR + 0x00);
	uint32_t* TIM1_EGR = (uint32_t*)(TIM1_BASE_ADDR + 0x14);
	uint32_t* TIM1_CCR1 = (uint32_t*)(TIM1_BASE_ADDR + 0x34);

	*TIM1_PSC = 16 - 1;
	*TIM1_ARR = 1000;

	*TIM1_CCMR1 &=~ ((0b1 << 3) | (0b111 << 4) );
	*TIM1_CCMR1 |= (0b1 << 3 ) | (0b110 << 4);

	*TIM1_EGR &=~ (0b1 << 0);
	*TIM1_EGR |= (0b1 << 0);

	*TIM1_CCER &=~ (0b1 << 0);
	*TIM1_CCER |= (0b1 << 0);

	*TIM1_BDTR &=~ ((0b1 << 10) | (0b1 << 11) | (0b1 << 15));
	*TIM1_BDTR |= (0b1 << 10) | (0b1 << 11) |(0b1 << 15);

	*TIM1_CR1 &=~ ((0b1 << 0) | (0b1 << 7));
	*TIM1_CR1 |= (0b1 << 7) | (0b1 << 0);

	*TIM1_CR1 &=~ (0b11 << 5);
//	*TIM1_CR1 |= (0b01 << 5);
}





/*---------------PUBLIC FUNCTIONS---------------*/

void Motor_Rota_Direction_Signal_Init(void){
	Motor_Direc_Signal_Config();
}

void Motor_Rota_Direction_Ctrl_Init(void){
	Motor_Rota_Direction_Ctrl_Config();
}

/*
 * Chân PB8 PB9 xuất tín hiệu điều khiển chiều quay động cơ
 */
void Motor_Direc_Ctrl(){
	volatile uint32_t* GPIOB_ODR = (uint32_t*)(GPIOB_BASE_ADDR + 0x14);
	if(rever == 0){
		*GPIOB_ODR |= (0b1 << 8);
		*GPIOB_ODR &=~ (0b1 << 9);
	}
	else if(rever == 1){
		*GPIOB_ODR |= (0b1 << 9);
		*GPIOB_ODR &=~ (0b1 << 8);
	}
}



/**
 * @brief Hàm khởi tạo TIM1 thành ngoại vi PWM
 */
void TIM1_PWM_Init(void){
	TIM1_PWM_Config();
}

/**
 * @brief Hàm điều khiển tốc độ động cơ
 */


void Adaptive_Speed_Limit(void){

}
/**
 * @brief Hàm cài đặt giá trị tốc độ quay tối đa động cơ
 */
void Motor_Contrl(void){
	volatile uint32_t* TIM1_CCR1 = (uint32_t*)(TIM1_BASE_ADDR + 0x34);

	val = ADC1_Get_Val();			//Đọc giá trị ADC từ chân PC0 của biến trở
	voltage = (val * 3.0)/4095;		//Chuyển đổi ADC thành giá trị điện áp (4095 -- 3V)
	duty_cycle = (voltage * 1000)/3; //Chuyển giá trị điện áp thành duty_cycle (100)
	*TIM1_CCR1 = duty_cycle;
}





