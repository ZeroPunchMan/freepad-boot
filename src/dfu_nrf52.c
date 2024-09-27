#include "dfu_nrf52.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include "flash_layout.h"
#include "crc.h"
#include "cl_log.h"

const struct device *flash_dev = FIXED_PARTITION_DEVICE(storage_partition);

//********************************flash操作******************************************
typedef struct
{
    uint32_t size;
    uint32_t hash;
} AppInfo_t;

CL_Result_t EraseAppSection(void)
{
    return EraseFlash(APP_START_ADDR, APP_MAX_SIZE / FLASH_PAGE_SIZE);
}

CL_Result_t SaveAppInfo(uint32_t addr, uint32_t size)
{
    AppInfo_t info;
    info.size = size;
    info.hash = Ethernet_CRC32((const uint8_t *)addr, size);
    EraseFlash(DFU_APP_INFO_ADDR, 1);
    WriteFlash(DFU_APP_INFO_ADDR, (const uint8_t *)&info, sizeof(info));

    return CL_ResSuccess;
}

bool IsAppValid(void)
{
    const AppInfo_t *pInfo = (const AppInfo_t *)DFU_APP_INFO_ADDR;

    if (pInfo->size > APP_MAX_SIZE)
        return false;

    uint32_t hash = Ethernet_CRC32((const uint8_t *)APP_START_ADDR, pInfo->size);

    CL_LOG_INFO("check app, size: %u, calc %x, save: %x", pInfo->size, hash, pInfo->hash);
    return hash == pInfo->hash;
}

CL_Result_t EraseFlash(uint32_t addr, uint32_t pages)
{
    return flash_erase(flash_dev, addr, pages * FLASH_PAGE_SIZE) == 0;
}

CL_Result_t WriteFlash(uint32_t addr, const uint8_t *buff, uint32_t length)
{
    return flash_write(flash_dev, addr, buff, length) != 0;
}

bool NeedDfu(void)
{
    return true;
}

CL_Result_t UnmarkDfu(void)
{
    return CL_ResSuccess;
}
