#pragma once

#define FLASH_PAGE_SIZE (4096ul)

#define BOOT_MAX_SIZE (160ul * 1024ul)
#define APP_MAX_SIZE (300ul * 1024ul)

#define BOOT_START_ADDR (0x0000000UL)
#define APP_START_ADDR (BOOT_START_ADDR + BOOT_MAX_SIZE)

#define DFU_APP_INFO_ADDR (APP_START_ADDR + APP_MAX_SIZE)
#define PAD_PARAM_ADDR (DFU_APP_INFO_ADDR + FLASH_PAGE_SIZE)
