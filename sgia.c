/**
  ******************************************************************************
  * @file           : sgia.c
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

/* Includes ------------------------------------------------------------------*/
#include "sgia.h"

#include "esp_log.h"

/* Private macros ------------------------------------------------------------*/
#define SGIA_DEFAULT_COMPENSATION_HUM		0x8000;
#define SGIA_DEFAULT_COMPENSATION_TEMP	0x6666

/* External variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "sgia";

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief Function that convert temperature physical temperature ticks
 *
 * @param temp : Temperature physical value
 *
 * @return Temperature ticks.
 */
static uint16_t convert_celsius_to_ticks(float temp);

/**
 * @brief Function that convert physical humidity to humidity ticks
 *
 * @param hum : Humidity physical value
 *
 * @return Humidity ticks.
 */
static uint16_t convert_percent_rh_to_ticks(float hum);

/* Exported functions definitions --------------------------------------------*/
/**
 * @brief Function to initialize a SGIA instance
 */
esp_err_t sgia_init(sgia_t *const me, i2c_master_bus_handle_t i2c_bus_handle) {
	ESP_LOGI(TAG, "Initializing SGIA instance...");

	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* Fill instance members */
	me->temp = 0.0f;
	me->hum = 0.0f;
	me->voc_raw = 0;
	me->nox_raw = 0;
	me->voc_index = 0;
	me->nox_index = 0;
	me->nox_conditioning = 10;

	/* Check I2C bus handle */
	if (i2c_bus_handle == NULL) {
		return ESP_FAIL;
	}

	/* Initialize SHT4x */
	ret = sht4x_init(&me->sht41, i2c_bus_handle, SHT41_I2C_ADDR_44);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Initialize SGP41 */
	ret = sgp41_init(&me->sgp41, i2c_bus_handle, SGP41_I2C_ADDR);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/**/
	GasIndexAlgorithm_init(&me->voc_params, GasIndexAlgorithm_ALGORITHM_TYPE_VOC);
	GasIndexAlgorithm_init(&me->nox_params, GasIndexAlgorithm_ALGORITHM_TYPE_NOX);

	/* Print success message */
	ESP_LOGI(TAG, "Initialized successfully");

	/* Return ESP_OK */
	return ret;
}

/**
 * @brief Function to get the temperature and humidity values
 */
void sgia_get_temp_and_hum(sgia_t *const me, float *temp, float *hum) {
	*temp = me->temp;
	*hum = me->hum;
}

/**
 * @brief Function to get the VOC and NOx raw values
 */
void sgia_get_raw_voc_and_nox(sgia_t *const me, uint16_t *voc_raw, uint16_t *nox_raw) {
	*voc_raw = me->voc_raw;
	*nox_raw = me->nox_raw;
}

/**
 * @brief Function to get the VOC and NOx index values
 */
void sgia_get_index_voc_and_nox(sgia_t *const me, int32_t *voc_index, int32_t *nox_index) {
	*voc_index = me->voc_index;
	*nox_index = me->nox_index;
}

/**
 * @brief Function to run the SGIA algorithm process
 */
esp_err_t sgia_run(sgia_t *const me) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

  /* Measure SHT4x  temperature and humidity signals and convert to SGP41 ticks */
	uint16_t temp_comp = 0;
	uint16_t hum_comp = 0;

	if (sht4x_measure_high_precision(&me->sht41, &me->temp, &me->hum) != ESP_OK) {
		temp_comp = SGIA_DEFAULT_COMPENSATION_TEMP;
		hum_comp = SGIA_DEFAULT_COMPENSATION_HUM;
	}
	else {
		temp_comp = convert_celsius_to_ticks(me->temp);
		hum_comp = convert_percent_rh_to_ticks(me->hum);
	}

	/* Measure SGP41 raw values */
	if (me->nox_conditioning > 0) {
		ret = sgp41_execute_conditioning(&me->sgp41, hum_comp, temp_comp, &me->voc_raw);
		if (ret != ESP_OK) {
			return ret;
		}
		me->nox_conditioning--;
	}
	else {
		ret = sgp41_measure_raw_signals(&me->sgp41, hum_comp, temp_comp, &me->voc_raw, &me->nox_raw);
		if (ret != ESP_OK) {
			return ret;
		}
	}

	/* Process raw values by Gas Index Algorithm to get the VOC and NOx index
	 * values */
	GasIndexAlgorithm_process(&me->voc_params, me->voc_raw, &me->voc_index);
	GasIndexAlgorithm_process(&me->nox_params, me->nox_raw, &me->nox_index);

  /* Return ESP_OK */
  return ret;
}

/* Private function definitions ----------------------------------------------*/
/**
 * @brief Function that convert temperature physical temperature ticks
 */
static uint16_t convert_celsius_to_ticks(float temp) {
	return (uint16_t)((temp + 45.0) * 65535.0 / 175.0);
}

/**
 * @brief Function that convert physical humidity to humidity ticks
 */
static uint16_t convert_percent_rh_to_ticks(float hum) {
	return (uint16_t)(hum * 65535.0 / 100.0);
}

/***************************** END OF FILE ************************************/
