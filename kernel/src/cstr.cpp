#include "cstr.h"

char uintTo_strOutput[128];
const char* to_str(uint64_t value)
{
    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest/10 > 0)
    {
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        uintTo_strOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintTo_strOutput[size - index] = remainder + '0';
    uintTo_strOutput[size + 1] = 0;

    return uintTo_strOutput;
}

char intTo_strOutput[128];
const char* to_str(int64_t value)
{
    uint8_t isNegative = 0;
    if (value < 0)
    {
        isNegative = 1;
        value *= -1;
        intTo_strOutput[0] = '-';
    }

    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest/10 > 0)
    {
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        intTo_strOutput[isNegative + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    intTo_strOutput[isNegative + size - index] = remainder + '0';
    intTo_strOutput[isNegative + size + 1] = 0;

    return intTo_strOutput;
}

char doubleTo_strOutput[128];
const char* to_str(double value, uint8_t decimalPlaces)
{
    if (decimalPlaces > 20) { decimalPlaces = 20; }

    char* intPtr = (char*)to_str((int64_t)value);
    char* doublePtr = doubleTo_strOutput;

    if (value < 0) {
        value *= -1;
    }

    while (*intPtr != 0)
    {
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    *doublePtr = '.';
    doublePtr++;

    double newValue = value - (int)value;

    for(uint8_t i  = 0; i < decimalPlaces; i++)
    {
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;
    return doubleTo_strOutput;
}

char hexTo_strOutput[128];
const char* to_hstr(uint64_t value)
{
    uint64_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 15;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_strOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? '7' : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_strOutput[size - (i * 2)] = tmp + (tmp > 9 ? '7' : '0');
    }

    hexTo_strOutput[size + 1] = 0;
    return hexTo_strOutput;
}


char hexTo_strOutput32[128];
const char* to_hstr(uint32_t value)
{
    uint32_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 7;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_strOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? '7' : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_strOutput32[size - (i * 2)] = tmp + (tmp > 9 ? '7' : '0');
    }

    hexTo_strOutput32[size + 1] = 0;
    return hexTo_strOutput32;
}

char hexTo_strOutput16[128];
const char* to_hstr(uint16_t value)
{
    uint16_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 3;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_strOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? '7' : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_strOutput16[size - (i * 2)] = tmp + (tmp > 9 ? '7' : '0');
    }

    hexTo_strOutput16[size + 1] = 0;
    return hexTo_strOutput16;
}

char hexTo_strOutput8[128];
const char* to_hstr(uint8_t value)
{
    uint8_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_strOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? '7' : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_strOutput8[size - (i * 2)] = tmp + (tmp > 9 ? '7' : '0');
    }

    hexTo_strOutput8[size + 1] = 0;
    return hexTo_strOutput8;
}

const char* to_str(double value)
{
    return to_str(value, 4);
}