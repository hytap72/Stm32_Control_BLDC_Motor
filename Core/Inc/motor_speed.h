/*
 * motor_speed.h
 *
 *  Created on: Nov 9, 2025
 *      Author: Admin
 */

#ifndef INC_MOTOR_SPEED_H_
#define INC_MOTOR_SPEED_H_


/*-------------FUNTIONS-------------*/

extern volatile float speed;
extern volatile uint32_t encoder_value;
extern volatile uint32_t revolutions, last_revolutions;


/**
 * @brief Khởi tạo TIM2 đọc tín hiệu Encoder 2 kênh A B
 * 		  Cấu hình ngát cho TIM2 đếm số vòng quay động cơ (1 vòng -- ARR 197)
 */
void TIM2_Encoder_Init(void);


/**
 * @brief Khởi tạo ngắt TIM4 để tính số vòng quay động cơ (rmp)
 */
void TIM4_Motor_Speed_Init(void);



/**
 * @brief Hàm đọc giá trị Encoder
 */
uint32_t Get_Val_Encoder();
#endif /* INC_MOTOR_SPEED_H_ */
