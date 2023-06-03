#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t block_t;

typedef struct {
    size_t x, y, z;
    block_t* blocks;
} WorldData;

#ifdef __cplusplus
}
#endif
