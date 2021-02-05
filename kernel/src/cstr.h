#pragma once
#include <stdint.h>

const char* to_str(uint64_t value);
const char* to_str(int64_t value);
const char* to_hstr(uint64_t value);
const char* to_hstr(uint32_t value);
const char* to_hstr(uint16_t value);
const char* to_hstr(uint8_t value);
const char* to_str(double value, uint8_t decimalPlaces);
const char* to_str(double value);