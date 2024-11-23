//
// Created by xhy on 2024/5/3.
//

#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <parallel_hashmap/phmap.h>

#include "loguru.hpp"

#define LE(...) LOG_F(ERROR, __VA_ARGS__)
#define LW(...) LOG_F(WARNING, __VA_ARGS__)
#define LI(...) LOG_F(0, __VA_ARGS__)
#define LD(...) LOG_F(1, __VA_ARGS__)

using tick_t = int64_t;
template <typename K, typename V>
using pfhmap = phmap::parallel_flat_hash_map<K, V>;
#endif
