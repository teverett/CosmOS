//*****************************************************************
// This file is part of CosmOS                                    *
// Copyright (C) 2020-2021 Tom Everett                            *
// Released under the stated terms in the file LICENSE            *
// See the file "LICENSE" in the source distribution for details  *
// ****************************************************************

#ifndef STRING_H
#define STRING_H

#include <cosmos_types.h>

uint64_t strlen(const uint8_t* s);
uint8_t* strtrim(const uint8_t* s);
uint8_t* strncpy(uint8_t* dest, const uint8_t* src, uint64_t len);
uint8_t* strncat(uint8_t* dest, const uint8_t* src, uint64_t len);
uint8_t strcmp(const uint8_t* str1, const uint8_t* str2);
uint8_t strncmp(const uint8_t* str1, const uint8_t* str2, uint64_t len);
uint32_t strstr(const uint8_t* str1, uint32_t start, const uint8_t* str2);
uint8_t* substr(const uint8_t* str1, uint32_t start, uint32_t end, uint8_t* str2, uint32_t size);

#endif