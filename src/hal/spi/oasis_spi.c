/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "oasis_spi.h"
#include "oasis_log.h"
#include "oasis_tools.h"

#define SPI_DEVICE       "/dev/spidev1.0"
#define SPI_SPEED        10000000
#define SPI_REG_LENGTH   8
#define SPI_REG_LOC      4
#define SPI_VALUE_LOC    5
#define SPI_DELAY        5

static pthread_spinlock_t spinlock;

int SPI_Read(uint8_t reg, uint8_t *value)
{
    int ret = SUCCESS;
    int fd;
    uint8_t tx[] = {0x55, 0x55, 0x40, 0x00, 0x00, 0x00};
    uint8_t rx[SPI_REG_LENGTH] = {0};

    struct spi_ioc_transfer transfer =
    {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = SPI_REG_LENGTH,
        .delay_usecs = SPI_DELAY,
        .bits_per_word = SPI_REG_LENGTH,
    };

    tx[SPI_REG_LOC] = reg;

    pthread_spin_lock(&spinlock);

    fd = open(SPI_DEVICE, O_RDWR);
    if (-1 == fd)
    {
        LOG_WARNNING("Cann't open spi file!");
        return ERROR;
    }

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
    if (0 > ret)
    {
        LOG_WARNNING("Failed to call ioctl");
        return ERROR;
    }

    *value = rx[SPI_VALUE_LOC];

    close(fd);

    pthread_spin_unlock(&spinlock);

    return SUCCESS;
}

int SPI_Write(uint8_t reg, uint8_t value)
{
    int ret = SUCCESS;
    int fd;
    uint8_t tx[] = {0x55, 0x55, 0x80, 0x00, 0x01, 0x01};
    uint8_t rx[SPI_REG_LENGTH] = {0};

    struct spi_ioc_transfer transfer =
    {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = SPI_REG_LENGTH,
        .delay_usecs = SPI_DELAY,
        .bits_per_word = SPI_REG_LENGTH,
    };

    tx[SPI_REG_LOC] = reg;
    tx[SPI_VALUE_LOC] = value;

    pthread_spin_lock(&spinlock);

    fd = open(SPI_DEVICE, O_RDWR);
    if (fd == -1)
    {
        LOG_WARNNING("Cann't open spi file!");
        return ERROR;
    }
    
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
    if (0 > ret)
    {
        LOG_WARNNING("Failed to call ioctl");
        return ERROR;
    }
    
    close(fd);

    pthread_spin_unlock(&spinlock);

    return SUCCESS;
}

int SPI_Init()
{
    pthread_spin_init(&spinlock, 0);

    return SUCCESS;
}

#ifdef __cplusplus
}
#endif