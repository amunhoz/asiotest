#pragma once

#ifndef ACROBATIC_UTILS_RANDOM
#define ACROBATIC_UTILS_RANDOM

#include <stdlib.h>
#include <string.h>
#include <random>


uint32_t gen_random_number();
std::string gen_random_string(const int len);
std::string gen_random_string_safe(const int len);

#endif 