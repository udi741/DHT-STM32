# DHT STM32 Library

This is a lightweight and easy-to-use library for interfacing with DHT11 humidity and temperature sensor on STM32 microcontrollers.

## Features

- Read temperature and humidity measurements from DHT11 sensor.
- Supports STM32 microcontrollers using STM32Cube HAL.

## Getting Started

1. Clone the repository or download the ZIP file.
2. Add the library files (`dht-stm32.c` and `dht-stm32.h`) to your project and include the `dht-stm32.h` header in your source code to utilize the library functions.
3. If you're not using STM32CubeMX for project setup, make sure to manually include the necessary STM32 header files (`stm32fxxxx.h`) and related HAL libraries in your project. These files provide the essential definitions and configurations for your STM32 microcontroller.
4. Configure your GPIO pins and timer in your project.
5. Initialize the DHT sensor using `DHTinit()` function, providing the required GPIO and Timer instances.

## Usage

### Example: Getting Temperature and Humidity Measurements

```c
#include "dht-stm32.h"

int main(void) {
    // Initialize your microcontroller's peripherals and clock settings.

    dht_t dht;

    // Configure your GPIO and Timer instances.
    // - `GPIOA`: The GPIO instance to which the DHT sensor's data pin is connected.
    // - `GPIO_PIN_1`: The GPIO pin number to which the DHT sensor's data line is connected.
    // - `htim2`: The Timer instance used for timing purposes during communication.
    // - `timerClockFreqMHz`: The clock frequency of the Timer, expressed in megahertz (MHz).
    DHTinit(&dht, GPIOA, GPIO_PIN_1, &htim2, /* timerClockFreqMHz */);

    while (1) {
        // Request temperature and humidity data from the DHT sensor.
        dhtData_t data = getDHTMeasurement(&dht);

        // Use data.temperature and data.humidity for further processing.

        // Print the measurements to the console or perform any desired action.
        // ...

        // Wait before taking the next measurement.
        // Ensure a minimum gap of 4 milliseconds between consecutive measurements.
        // Delay as needed using your microcontroller's delay function.
        // ...
    }
}
```
## Initialization Command

The `DHTinit()` function is used to initialize the DHT sensor. It expects the following parameters:

```c
void DHTinit(dht_t* dht, GPIO_TypeDef* gpio, uint16_t gpio_pin, TIM_HandleTypeDef* timer, uint16_t timerClockFreqMHz);
```
- `dht`: A pointer to a `dht_t` structure where the DHT sensor's configuration and data will be stored.
- `gpio`: The GPIO instance (e.g., `GPIOA`, `GPIOB`, etc.) to which the DHT sensor is connected.
- `gpio_pin`: The GPIO pin number to which the DHT sensor's data line is connected.
- `timer`: The Timer instance used for timing purposes during communication.
- `timerClockFreqMHz`: The clock frequency of the Timer, expressed in megahertz (MHz), based on your STM32Cube HAL configuration.

## Notes
- <ins>**Important:**</ins> Ensure there is a minimum gap of **4 milliseconds** between consecutive measurements to prevent inaccurate readings.
- The library supports FreeRTOS (`USINGFREERTOS` defined as 1 in `dht-stm32.h`).
- Make sure to define the GPIO pin and Timer instance that you plan to use with STM32Cube HAL before initializing the DHT sensor.
- The library uses GPIO output mode and interrupt-driven input mode to communicate with the DHT sensor.

## Contributors

- [Udi](https://github.com/udi741)
