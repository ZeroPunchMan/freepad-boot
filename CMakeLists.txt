cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(freepad)
include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)


include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/common/clib/inc)
include_directories(${CMAKE_SOURCE_DIR}/common/mmlib/inc)

FILE(GLOB app_sources src/*.c)
FILE(GLOB clib_sources common/clib/src/*.c)

set(mmlib_sources common/mmlib/src/sgp_protocol.c common/mmlib/src/systime.c common/mmlib/src/crc.c)

set(SOURCES
	${app_sources}
	${clib_sources}
	${mmlib_sources}
)

target_sources(app PRIVATE ${SOURCES})

