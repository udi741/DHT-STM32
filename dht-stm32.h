#ifndef __DHT_STM32_H__
#define __DHT_STM32_H__

#ifdef __cplusplus
 extern "C" {
#endif

#define USINGFREERTOS 1

#include "main.h"
#include <stdbool.h>
#if (USINGFREERTOS == 1)
	#include "cmsis_os.h"
	#define DHTDelay(t)	osDelay(t)
#else
	#define DHTDelay(t)	HAL_Delay(t)
#endif

typedef struct
{
	float temperature;
	float humidity;
}dhtData_t;

typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
	TIM_HandleTypeDef* timer;
	uint16_t lastCnt;
	uint8_t binaryData[2*2+40*2];
	uint16_t msgInterruptCnt;	
	dhtData_t data;
}dht_t;

void DHTinit(dht_t* dht, GPIO_TypeDef* gpio, uint16_t gpio_pin, TIM_HandleTypeDef* timer, uint16_t timerClockFreqMHz);
void DHTRequestData(dht_t* dht);
void DHTConfigureToOutput(dht_t* dht);
void DHTConfigureToInput(dht_t* dht);
uint32_t DHTGetTimerCnt(dht_t* dht);
void DHTSetTimerCnt(dht_t* dht, uint32_t timeToSet);
void DHTInterruptCallback(dht_t* dht);
bool DHTReadData(dht_t* dht, uint8_t* data);
dhtData_t getDHTMeasurement(dht_t* dht);
int32_t measureDeltaUs(dht_t* dht);

#ifdef __cplusplus
}
#endif
#endif
