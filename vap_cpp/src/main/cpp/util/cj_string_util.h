/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#ifndef LIBPAG4CJ_CJ_STRING_UTIL_H
#define LIBPAG4CJ_CJ_STRING_UTIL_H

#include "data_info.h"
#include <cstdint>
extern "C"{
namespace pag {
CJBytesArray stringToCJBytesArray(const std::string &str);
void safe_copy(void *dest,uint32_t dest_size, const void *src, uint32_t src_size);
}
}
#endif // LIBPAG4CJ_CJ_STRING_UTIL_H
