#pragma once
#include <CC/type.h>

#define CC_WIDX(ax, ay, az, wData) (   (ay) * (wData)->x * (wData)->z \
                                    + (az) * (wData)->x \
                                    + (ax))

#ifdef __cplusplus
extern "C" {
#endif

void CC_World_Init(size_t x, size_t y, size_t z);
void CC_World_Term(void);

void CC_World_Generate(void);

void CC_World_GetChunkSize(size_t* x, size_t* y, size_t* z);
void CC_World_GetSize(size_t* x, size_t* y, size_t* z);

const WorldData* CC_World_GetData(void);

void CC_World_GetBlock(size_t x, size_t y, size_t z, block_t* block);
bool CC_World_TryGetBlock(size_t x, size_t y, size_t z, block_t* block);
void CC_World_SetBlock(size_t x, size_t y, size_t z, block_t block);

void CC_World_Save(void);
bool CC_World_Load(void);

#ifdef __cplusplus
}
#endif
