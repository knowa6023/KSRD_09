/*
 * application.c
 *
 *  Created on: Feb 15, 2022
 *      Author: FM
 */
#include <stdio.h>
#include <string.h>
#include "application.h"
#include "bmp280.h"
#include "gps.h"
#include "w25qxx.h"
#include <stdio.h>
#include "global.h"
#include "fatfs.h"
#include "strings.h"
#include "stdbool.h"

extern GPS_t GPS;
extern w25qxx_t	w25qxx;

uint32_t byteswritten, bytesread;
uint8_t result;
extern char USERPath[4]; /* USER logical drive path */
FATFS SDFatFs;
FATFS *fs;
FIL MyFile;

static uint32_t MakeFile(TYPE_FILE type, const TCHAR* nameFile, const void* inStruct, uint32_t sizeStruct);



extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c2;
extern RTC_HandleTypeDef hrtc;
static uint8_t msg_1[] ="<FM>Message_1<END>\r\n";
//
////-------------------------------------------------BMP280--------------------------------------------
BMP280_HandleTypedef bmp280;
static float pressure, temperature, humidity;

uint16_t size;
uint8_t Data[128];
////-------------------------------------------------BMP280--------------------------------------------
//
////-------------------------------------------------RTC--------------------------------------------
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef DateToUpdate = {0};
char trans_str[64] = {0,};
////-------------------------------------------------RTC--------------------------------------------
//
//extern void GPS_Init();

Data_RouteFileTypeDef data_RouteFile={0};


void App_main(void)
{

	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c2;

	while (!bmp280_init(&bmp280, &bmp280.params))
	{
			size = sprintf((char *)Data, "BMP280 initialization failed \r\n");
			UART_1_Transmit_IT(Data,size);
			HAL_Delay(2000);
	}
	bool bme280p = bmp280.id == BME280_CHIP_ID;
	size = sprintf((char *)Data, "BMP280: found %s \r\n", bme280p ? "BME280" : "BMP280");
//	UART_1_Transmit_IT(Data,size);
//
	DWT_Init();
	GPS_Init();
//	W25qxx_Init();
//	W25qxx_EraseChip();
//	  uint8_t b0 = 's';
//	  uint8_t b1 = 't';
//	  uint8_t b2 = 'D';
//
//	  W25qxx_WriteByte(b0, 25);
//	  W25qxx_WriteByte(b1, 26);
//	  W25qxx_WriteByte(b2, 27);
//
//	  uint8_t mas[3]={0,};
//	  W25qxx_ReadByte(&mas[0], 25);
//	  W25qxx_ReadByte(&mas[0], 26);
//	  W25qxx_ReadByte(&mas[0], 27);

		uint8_t nameFile[20];
		uint8_t iFile=0;
	while(1)
	{
		HAL_Delay(1);

		//strcpy(name_file,"route_",(char)i);
		sprintf((char*)nameFile,"route_%d.bin",iFile);
		MakeFile(FILE_ROUTE,(const TCHAR*)nameFile, &data_RouteFile, sizeof(data_RouteFile));
		iFile++;
		bmp280_read_int32_t(&bmp280, &data_RouteFile.temperature_BMP280, &data_RouteFile.pressure_BMP280);
		//bmp280_read_float(&bmp280, &temperature, &pressure, &humidity);
//		if(iFile>=250)
//		{
//			while(1)
//			{
//				HAL_Delay(1);
//			}
//		}
//        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); // RTC_FORMAT_BIN , RTC_FORMAT_BCD
//        snprintf(trans_str, 63, "Time %d:%d:%d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
//		UART_1_Transmit_IT(trans_str,strlen(trans_str));
//
//        HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
//        snprintf(trans_str, 63, "Date %d-%d-20%d\n", DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year);
//		UART_1_Transmit_IT(trans_str,strlen(trans_str));
//
//
//		  HAL_Delay(200);
////		  App_UART_1_Transmit_IT(msg_1,sizeof(msg_1)-1);
//		  HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);


//----------------------------------------------------------------------------------------



//		uint32_t buf =GPS.date;
//		while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
//		{
//			size = sprintf((char *)Data,"Temperature/pressure reading failed \r\n");
//			UART_1_Transmit_IT(Data,size);
//			HAL_Delay(2000);
//		}


//
//		size = sprintf((char *)Data,"Pressure: %.2f Pa,Temperature: %.2f \r\n", pressure, temperature);
//		UART_1_Transmit_IT(Data,size-1);
//
//		if (bme280p)
//		{
//			size = sprintf((char *)Data,", Humidity: %.2f \r\n", humidity);
//			UART_1_Transmit_IT(Data,size);
//		}
//		else
//		{
//			size = sprintf((char *)Data, "\r\n");
//			UART_1_Transmit_IT(Data,size);
//		}
	//	HAL_Delay(500);
	}
}




void UART_1_Transmit_IT(uint8_t str[],uint16_t len)
{
	//HAL_UART_Transmit_IT(&huart1, str,len);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart3) GPS_UART_CallBack();
}


static inline void DWT_Init(void)
{
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;// разрешаем использовать DWT
	DWT_CONTROL|= DWT_CTRL_CYCCNTENA_Msk; // включаем счётчик

}
static inline void DWT_Clear(void)
{
	DWT_CYCCNT = 0;// обнуляем счётчик
}
static inline uint32_t DWT_Get_Value(void)
{
	return DWT_CYCCNT;
}

static uint32_t MakeFile(TYPE_FILE type, const TCHAR* nameFile, const void* inStruct, uint32_t sizeStruct)
{
	uint32_t res= f_mount(&SDFatFs,(TCHAR const*)USERPath,1);
	uint32_t byteswritten;
	if(res!=FR_OK)
	{
		Error_Handler();
	}
	else
	{
		if(f_open(&MyFile,(const TCHAR*)nameFile,FA_CREATE_ALWAYS|FA_WRITE)!=FR_OK)
		{
			Error_Handler();
		}
		else
		{
			res=f_write(&MyFile,(const void*) inStruct, sizeStruct, (void*)&byteswritten);
			if((byteswritten==0)||(res!=FR_OK))
			{
				Error_Handler();
			}
			f_close(&MyFile);
		}
	}
	return res;
}




