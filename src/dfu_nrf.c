
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include "dfu_nrf.h"

#define FLASH_PAGE_SIZE   4096 //todo 从设备树获取

const struct device *flash_dev = FIXED_PARTITION_DEVICE(storage_partition);

// int NrfFlashErase(uint32_t addr, uint32_t size)
// {

// }

// int NrfFlashWrite(uint32_t addr, const uint8_t* data, uint16_t len)
// {

// }

int NrfFlashRead(uint32_t addr, uint8_t* buff, uint16_t len)
{
    return flash_read(flash_dev, addr, buff, len);
}
