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
#ifndef GIA_H_
#define GIA_H_

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
} gia_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
esp_err_t gia_init(gia_t *const me, i2c_bus_t *i2c_bus);

void gia_get_temp_and_hum(gia_t *const me, float *temp, float *hum);

void gia_get_raw_voc_and_nox(gia_t *const me, uint16_t *voc_raw, uint16_t *nox_raw);

void gia_get_index_voc_and_nox(gia_t *const me, int32_t *voc_index, int32_t *nox_index);

esp_err_t gia_run(gia_t *const me);

#ifdef __cplusplus
}
#endif

#endif /* GIA_H_ */

/***************************** END OF FILE ************************************/
