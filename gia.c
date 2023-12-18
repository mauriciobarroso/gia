/**
  ******************************************************************************
  * @file           : gia.c
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
#include "gia.h"

#include "esp_log.h"

/* Private macros ------------------------------------------------------------*/
#define GIA_DEFAULT_COMPENSATION_HUM	0x8000;
#define GIA_DEFAULT_COMPENSATION_TEMP	0x6666

/* External variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "gia";

/* Private function prototypes -----------------------------------------------*/
static void gia_get_compesation_values(gia_t *const me, uint16_t *temp_comp, uint16_t *hum_comp);

/* Exported functions definitions --------------------------------------------*/
esp_err_t gia_init(gia_t *const me, i2c_bus_t *i2c_bus) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

	/* */
	me->temp = 0.0f;
	me->hum = 0.0f;
	me->voc_raw = 0;
	me->nox_raw = 0;
	me->voc_index = 0;
	me->nox_index = 0;
	me->nox_conditioning = 10;

	/**/
	if (i2c_bus == NULL) {
		return ESP_FAIL;
	}

	/* Initialize SHT4x */
	ret = sht4x_init(&me->sht41, i2c_bus, SHT41_I2C_ADDR_44, NULL, NULL);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/* Initialize SGP41 */
	ret = sgp41_init(&me->sgp41, i2c_bus, SGP41_I2C_ADDR, NULL, NULL);

	if (ret != ESP_OK) {
		return ESP_FAIL;
	}

	/**/
	GasIndexAlgorithm_init(&me->voc_params, GasIndexAlgorithm_ALGORITHM_TYPE_VOC);
	GasIndexAlgorithm_init(&me->nox_params, GasIndexAlgorithm_ALGORITHM_TYPE_NOX);

	/* Return ESP_OK */
	return ret;
}

void gia_get_temp_and_hum(gia_t *const me, float *temp, float *hum) {
	*temp = me->temp;
	*hum = me->hum;
}

void gia_get_raw_voc_and_nox(gia_t *const me, uint16_t *voc_raw, uint16_t *nox_raw) {
	*voc_raw = me->voc_raw;
	*nox_raw = me->nox_raw;
}

void gia_get_index_voc_and_nox(gia_t *const me, int32_t *voc_index, int32_t *nox_index) {
	*voc_index = me->voc_index;
	*nox_index = me->nox_index;
}

esp_err_t gia_run(gia_t *const me) {
	/* Variable to return error code */
	esp_err_t ret = ESP_OK;

  /* Measure SHT4x  RH and T signals and convert to SGP41 ticks */
	uint16_t temp_comp = 0;
	uint16_t hum_comp = 0;
	gia_get_compesation_values(me, &temp_comp, &hum_comp);

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
static void gia_get_compesation_values(gia_t *const me, uint16_t *temp_comp, uint16_t *hum_comp) {
	esp_err_t ret = sht4x_measure_high_precision(&me->sht41, &me->temp, &me->hum);

	if (ret != ESP_OK) {
		*temp_comp = GIA_DEFAULT_COMPENSATION_TEMP;
		*hum_comp = GIA_DEFAULT_COMPENSATION_HUM;
	}
	else {
		*temp_comp = (uint16_t)(me->temp + 45) * 65535 / 175;;
		*hum_comp = (uint16_t)me->hum * 65535 / 100;
	}
}

/***************************** END OF FILE ************************************/
