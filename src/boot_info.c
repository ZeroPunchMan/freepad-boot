#include "firmware_info.h"

#define MAJOR_VER_NUMBER (0)
#define MINOR_VER_NUMBER (0)
#define PATCH_VER_NUMBER (1)

const FirmwareInfo_t bootFwInfo = {
    .verMajor = MAJOR_VER_NUMBER,
    .verMinor = MINOR_VER_NUMBER,
    .verPatch = PATCH_VER_NUMBER,

    .check = FIRMWARE_CHECK_VALUE(MAJOR_VER_NUMBER, MINOR_VER_NUMBER, PATCH_VER_NUMBER, MINOR_VER_NUMBER, PATCH_VER_NUMBER),
};

// const int testData __attribute__((at(APP_START_ADDR + 38*1024))) = 0x12345;
