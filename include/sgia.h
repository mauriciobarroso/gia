/**
  ******************************************************************************
  * @file           : gia.h
  * @author         : Mauricio Barroso Benavides
  * @date           : Dec 17, 2023
  * @brief          : todo: write brief 
  ******************************************************************************
  * @attention
  *
  * MIT License
  *
  * Copyright (c) 2023 Mauricio Barroso Benavides
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to
  * deal in the Software without restriction, including without limitation the
  * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  * sell copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  * 
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  * IN THE SOFTWARE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SGIA_H_
#define SGIA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "sensirion_gas_index_algorithm.h"

#include "esp_err.h"

#include "sht4x.h"
#include "sgp41.h"

/* Exported Macros -----------------------------------------------------------*/

/* Exported typedef ----------------------------------------------------------*/
typedef struct {
	GasIndexAlgorithmParams voc_params;
	GasIndexAlgorithmParams nox_params;
	sht4x_t sht41;
	sgp41_t sgp41;
	float temp;
	float hum;
	uint16_t voc_raw;
	uint16_t nox_raw;
	int32_t voc_index;
	int32_t nox_index;
	uint8_t nox_conditioning;
} sgia_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Function to initialize a SGIA instance
 *
 * @param me             : Pointer to a sgia_t structure
 * @param i2c_bus_handle : Handle to I2C bus instance to add this device
 *
 * @return ESP_OK on success
 */
esp_err_t sgia_init(sgia_t *const me, i2c_master_bus_handle_t i2c_bus_handle);

/**
 * @brief Function to get the temperature and humidity values
 *
 * @param me   : Pointer to a sgia_t structure
 * @param temp : Pointer to variable to store the temperature value in celcius
 * @param hum  : Pointer to variable to store the humidity value
 */
void sgia_get_temp_and_hum(sgia_t *const me, float *temp, float *hum);

/**
 * @brief Function to get the VOC and NOx raw values
 *
 * @param me      : Pointer to a sgia_t structure
 * @param voc_raw : Pointer to variable to store the VOC raw value
 * @param nox_raw : Pointer to variable to store the NOx raw value
 */
void sgia_get_raw_voc_and_nox(sgia_t *const me, uint16_t *voc_raw, uint16_t *nox_raw);

/**
 * @brief Function to get the VOC and NOx index values
 *
 * @param me        : Pointer to a sgia_t structure
 * @param voc_index : Pointer to variable to store the VOC index value
 * @param nox_index : Pointer to variable to store the NOx index value
 */
void sgia_get_index_voc_and_nox(sgia_t *const me, int32_t *voc_index, int32_t *nox_index);

/**
 * @brief Function to run the SGIA algorithm process
 *
 * @param me : Pointer to a sgia_t structure
 *
 * @return ESP_OK on success
 */
esp_err_t sgia_run(sgia_t *const me);

#ifdef __cplusplus
}
#endif

#endif /* SGIA_H_ */

/***************************** END OF FILE ************************************/
