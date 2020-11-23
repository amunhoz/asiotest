#ifndef ACROBATIC_UTILS_TIME
#define ACROBATIC_UTILS_TIME

#include <cstdint>
#include <string>
#if !defined(_WIN32)
#include <sys/time.h>
#else
#include <Windows.h>
#endif

uint64_t getCurrentMs64();
uint32_t getCurrentMs();

#endif 