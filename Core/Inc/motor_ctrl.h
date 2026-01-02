/*
 * motor.h
 *
 *  Created on: Nov 8, 2025
 *      Author: Admin
 *
 */

#ifndef INC_MOTOR_CTRL_H_
#define INC_MOTOR_CTRL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/*	Export defines*/

extern float voltage;
extern uint16_t duty_cycle;
/*	Export funtions */

/**
 * @brief Khởi tạo nút bấm PB0 làm tín hiệu đảo chiều quay động cơ
 * @note Phải gọi để có được tín hiệu đảo chiều quay
 */
void Motor_Rota_Direction_Signal_Init(void);


/**
 * @brief Khởi tạo PB8 PB9 thành ouput điều khiển IN1 và IN2 mạch L295N
 * @note PB8 = 1 & PB9 = 0 --> Quay cùng chiều kim đồng hồ
 * 		 PB8 = 0 & PB9 = 1 --> Quay ngược chiều kim đồng hồ
 * 		 PB8 = 0 & PB9 = 0 --> Động cơ không quay
 * 		 PB8 = 1 & PB9 = 1 --> Phanh động cơ
 */
void Motor_Rota_Direction_Ctrl_Init(void);


/**
 * @brief Dựa trên tín hiệu ngắt ngoài của PB0 để quyết định chiều quay động cơ
 * @parameter rever = --> Quay cùng chiều kim đồng hồ
 * 			  rever = --> Quay ngược chiều kim đồng hồ
 */
void Motor_Direc_Ctrl(void);


/**
 * @brief Hàm khởi tạo TIM1 (CH1 - PE9) thành ngoại vi PWM điều khiển tốc độ động cơ
 * @note Cần gọi hàm này trước để có thể xuât xung PWM
 * 		 Chỉ cần khởi tạo chân sẽ liê tục xuất xung mà không cần hàm chạy trong vòng lặp
 */
void TIM1_PWM_Init(void);

/**
 * @brief Hàm điều khiển tốc độ động cơ
 */
void Motor_Contrl(void);

#ifdef __cplusplus
}
#endif
#endif /* INC_MOTOR_CTRL_H_ */
