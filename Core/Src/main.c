#include "motor_ctrl.h"
#include "main.h"
#include "rheostat.h"
#include "motor_speed.h"


int main(){
	HAL_Init();
	ADC_Init();
	Motor_Rota_Direction_Ctrl_Init();
	TIM1_PWM_Init();
	TIM4_Motor_Speed_Init();
	TIM2_Encoder_Init();
	Motor_Rota_Direction_Signal_Init();
	while(1){
		uint16_t val = ADC1_Get_Val();
		voltage = (val * 3.0)/4095;
		encoder_value = Get_Val_Encoder();
		Motor_Contrl();
		Motor_Direc_Ctrl();
	}
}
