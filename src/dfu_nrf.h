#pragma once

#include "stdint.h"

int NrfFlashRead(uint32_t addr, uint8_t* buff, uint16_t len);