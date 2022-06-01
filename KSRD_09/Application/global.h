/*
 * global.h
 *
 *  Created on: 1 июн. 2022 г.
 *      Author: FM
 */
#include "main.h"

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define    DWT_CYCCNT    *(volatile uint32_t*)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t*)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t*)0xE000EDFC

extern UART_HandleTypeDef huart3;


#define GPS_DEBUG	0
#define	GPS_USART	&huart3
#define GPSBUFSIZE  128       // GPS buffer size




//------------------------------------------------------------------------------------------


//BPLA
typedef struct
{
	uint8_t BPLA_DateTime[19]; //19
	uint8_t gps_str[700];  //700
	uint32_t heightFly;  //4
    uint32_t temperature;//4
}BPLA_TypeDef;


// Total
typedef struct
{
	uint8_t dataTime_RTC[19];			//19 byte
	uint8_t gps_str[700];  				//700 byte
	uint32_t temperature_BMP280; 		//4 byte // реальное значение равно = temperature/100
	uint32_t pressure_BMP280; 			//4 byte // реальное значение равно = pressure/256
	uint8_t BD_data[45];		        //45 byte
	BPLA_TypeDef data_BPLA;		        //727 byte
} Data_RouteFileTypeDef;

#endif /* GLOBAL_H_ */
