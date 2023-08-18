#include "dht-stm32.h"

void DHTinit(dht_t* dht, GPIO_TypeDef* gpio, uint16_t gpio_pin, TIM_HandleTypeDef* timer, uint16_t timerClockFreqMHz)
{
	dht->gpio = gpio;
	dht->pin = gpio_pin;
	dht->msgInterruptCnt = 0;
	dht->timer = timer;
	DHTConfigureToOutput(dht);
	// TODO: check if user configured the timer to inc cnt every 1 micro second, and the period is defined to max.
	// for now - assume that the timer can be configured by the library, but this might affect the users if they are using the timer for multiple things (for exmple, using the timer interrupt)
	dht->timer->Instance = TIM2;
  dht->timer->Init.Prescaler = timerClockFreqMHz-1;
  dht->timer->Init.CounterMode = TIM_COUNTERMODE_UP;
  dht->timer->Init.Period = 65535;
  dht->timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  dht->timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(dht->timer) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_TIM_Base_Init(dht->timer);
  HAL_TIM_Base_Start(dht->timer);
	
}

uint32_t DHTGetTimerCnt(dht_t* dht)
{
	return dht->timer->Instance->CNT;
}

void DHTSetTimerCnt(dht_t* dht, uint32_t timeToSet)
{
	dht->timer->Instance->CNT = timeToSet;
}

void DHTConfigureToInput(dht_t *dht)
{
	GPIO_InitTypeDef gpioInitRef;
	gpioInitRef.Pin = dht->pin;
  gpioInitRef.Mode = GPIO_MODE_IT_RISING_FALLING;
  gpioInitRef.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(dht->gpio, &gpioInitRef);
}

void DHTConfigureToOutput(dht_t* dht)
{
	GPIO_InitTypeDef gpioInitRef;
	gpioInitRef.Pin = dht->pin;
	gpioInitRef.Mode = GPIO_MODE_OUTPUT_OD;
	gpioInitRef.Pull = GPIO_NOPULL;
	gpioInitRef.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(dht->gpio, &gpioInitRef);
}

void DHTRequestData(dht_t* dht)
{
	DHTConfigureToOutput(dht);
	dht->gpio->ODR &= ~dht->pin;
	DHTDelay(18);
	dht->gpio->ODR |= dht->pin;
	dht->lastCnt = DHTGetTimerCnt(dht);
	while(measureDeltaUs(dht) < 40); // wait max time, in order to make sure the exti don't catch the host signal change
	dht->msgInterruptCnt = 0;
	dht->lastCnt = DHTGetTimerCnt(dht);
	DHTConfigureToInput(dht);
}

int32_t measureDeltaUs(dht_t* dht)
{
	int32_t timeDelta = DHTGetTimerCnt(dht) - dht->lastCnt;
		if (timeDelta < 0) // protect from negative time
			timeDelta += 0xFFFF;
	return timeDelta;
}

void DHTInterruptCallback(dht_t* dht)
{
	if (dht->msgInterruptCnt < sizeof(dht->binaryData)-1)
	{
		dht->binaryData[dht->msgInterruptCnt] = measureDeltaUs(dht);
		dht->lastCnt = DHTGetTimerCnt(dht);
		dht->msgInterruptCnt += 1;
	}
}

bool DHTReadData(dht_t* dht, uint8_t* data)
{
	uint8_t bitPos = 8;
	if (dht->binaryData[0] < 60 || dht->binaryData[0] > 85 || dht->binaryData[1] < 75 || dht->binaryData[1] > 85)
	{
		return false;
	}
	for(int i=0; i<sizeof(dht->binaryData)-4; i+=2)
	{
		if (dht->binaryData[i+2] < 40 || dht->binaryData[i+3] < 20 || dht->binaryData[i+3] > 80)
			return false;
		else
		{
			bitPos--;
			if (dht->binaryData[i+3] >= 65 && dht->binaryData[i+3] <= 80)
				*data |= (1 << bitPos);
			else if (dht->binaryData[i+3] >= 20 && dht->binaryData[i+3] <= 30)
				*data &= ~(1<<bitPos);
		}
		if (bitPos == 0)
		{
			data++;
			bitPos = 8;
		}
	}
	return true;
}

dhtData_t getDHTMeasurement(dht_t* dht)
{
	DHTRequestData(dht);
	uint8_t data[5];
	if (DHTReadData(dht, data) == true)
	{
		if (((data[0] + data[1] + data[2] + data[3]) & 0x00FF) == data[4])
		{
			dht->data.humidity = (float)(data[0] << 8 | data[1]) / 10.0f;
			dht->data.temperature = (float)(data[2] << 8 | data[3]) / 10.0f;
		}
	}
	return dht->data; // return data in any case - if it faild the check sum the user will get the last record.
}
