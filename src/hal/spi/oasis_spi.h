/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifndef _OASIS_SPI_H_
#define _OASIS_SPI_H_

#ifdef __cplusplus
extern "C"
{
#endif

int SPI_Init();

int SPI_Read(uint8_t reg, uint8_t *value);

int SPI_Write(uint8_t reg, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif
