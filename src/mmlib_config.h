#pragma once

#include "cl_common.h"

typedef enum
{
    SpgChannelHandle_Acm = 0,
    SpgChannelHandle_Max,
} SpgChannelHandle_t;


//-------------------tick from zephyr-----------------------------/
#include "zephyr/kernel.h"
#define MM_USE_DEFAULT_SYSTIME (0)

static inline uint32_t SysTimeSpan(uint32_t base)
{
    uint32_t sysTime = k_uptime_get_32();

    if (sysTime >= base)
        return sysTime - base;
    else
        return UINT32_MAX - base + sysTime + 1; 
}

static inline uint32_t GetSysTime(void)
{
    return k_uptime_get_32();
}

static inline void DelayOnSysTime(uint32_t time)
{
    k_msleep(time);
}
