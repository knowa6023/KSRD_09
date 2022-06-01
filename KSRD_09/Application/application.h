/*
 * application.h
 *
 *  Created on: Feb 15, 2022
 *      Author: FM
 */

#include "main.h"

#ifndef APPLICATION_H_
#define APPLICATION_H_



void App_main(void);
void UART_1_Transmit_IT(uint8_t str[],uint16_t len);


static inline void DWT_Init(void);
static inline void DWT_Clear(void);
static inline uint32_t DWT_Get_Value(void);

#endif /* APPLICATION_H_ */
