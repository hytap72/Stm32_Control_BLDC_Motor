/*
 * adc.h
 *
 *  Created on: Nov 8, 2025
 *      Author: Admin
 */

#ifndef INC_RHEOSTAT_H_
#define INC_RHEOSTAT_H_

#include <stdint.h>

/*-------------FUNTIONS-------------*/


/*
 * @brief Khởi tạo ngoại vi ADC trên chân PC1 để đọc giá trị điện áp đầu vào từ biến trở
 */
void ADC_Init(void);


/*
 * 	@brief Hàm đọc giá trị ADC1
 */
uint16_t ADC1_Get_Val();
#endif /* INC_RHEOSTAT_H_ */
