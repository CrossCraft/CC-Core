#pragma once
#include <stdint.h>
#include <stddef.h>

#if __cplusplus
extern "C" {
#endif

// Defines a function pointer type for reading data from a data bus
typedef size_t (*ReadFn)(uint8_t** data, void* context);

// Defines a function pointer type for writing data to a data bus
typedef size_t (*WriteFn)(uint8_t* data, size_t size, void* context);

typedef struct {
    ReadFn read;
    WriteFn write;
} DataBus;

#define SHARED_MEMORY_SIZE 1024
typedef struct {
    uint8_t data[SHARED_MEMORY_SIZE];
    int messagesReady;
    size_t messageWriteOffset;
    size_t messageReadOffset;
} SharedMemoryData;

typedef struct {
    DataBus bus;
    SharedMemoryData sharedMemoryData;
} SharedDataBus;

SharedDataBus* SharedDataBus_Init(void);
void SharedDataBus_Destroy(SharedDataBus* bus);

size_t SharedDataBus_Read(uint8_t** data, void* context);
size_t SharedDataBus_Write(uint8_t* data, size_t size, void* context);

#ifdef __cplusplus
}
#endif

