#pragma once

#include "cl_common.h"

void JumpToApp(void);

void Dfu_Init(void);
void Dfu_Process(void);

void Dfu_SendTest(const char *format, ...);


