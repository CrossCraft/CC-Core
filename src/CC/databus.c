#include <CC/databus.h>
#include <stdlib.h>
#include <string.h>

SharedDataBus* SharedDataBus_Init(void) {
    SharedDataBus *bus = malloc(sizeof(SharedDataBus));
    if (bus == NULL) {
        return NULL; // allocation failed
    }
    memset(bus, 0, sizeof(SharedDataBus));
    bus->bus.read = SharedDataBus_Read;
    bus->bus.write = SharedDataBus_Write;
    return bus;
}

void SharedDataBus_Destroy(SharedDataBus* bus) {
    free(bus);
}

size_t SharedDataBus_Read(uint8_t** data, void* context) {
    if (context == NULL) {
        return 0;
    }

    SharedDataBus* bus = (SharedDataBus*) context;
    if (bus->sharedMemoryData.messagesReady == 0) {
        return 0;
    }

    // Get encoded message length
    size_t length;
    memcpy(&length, &bus->sharedMemoryData.data[bus->sharedMemoryData.messageReadOffset], sizeof(size_t));

    // Copy message data
    *data = malloc(length);
    if(*data == NULL) {
        return 0;
    }

    memcpy(*data, &bus->sharedMemoryData.data[bus->sharedMemoryData.messageReadOffset + sizeof(size_t)], length);

    // Update offsets
    bus->sharedMemoryData.messagesReady--;
    bus->sharedMemoryData.messageReadOffset += sizeof(size_t) + length;

    // Reset offsets if we've read all messages
    if(bus->sharedMemoryData.messagesReady == 0) {
        bus->sharedMemoryData.messageReadOffset = 0;
        bus->sharedMemoryData.messageWriteOffset = 0;
    }

    return length;
}

size_t SharedDataBus_Write(uint8_t* data, size_t size, void* context) {
    if(context == NULL) {
        return 0;
    }

    if(data == NULL) {
        return 0;
    }

    SharedDataBus* bus = (SharedDataBus*) context;
    if(bus->sharedMemoryData.messageWriteOffset + size + sizeof(size_t) > SHARED_MEMORY_SIZE) {
        return 0;
    }

    // Copy message length
    memcpy(&bus->sharedMemoryData.data[bus->sharedMemoryData.messageWriteOffset], &size, sizeof(size_t));

    // Copy message data
    memcpy(&bus->sharedMemoryData.data[bus->sharedMemoryData.messageWriteOffset + sizeof(size_t)], data, size);

    // Update offsets
    bus->sharedMemoryData.messagesReady++;
    bus->sharedMemoryData.messageWriteOffset += sizeof(size_t) + size;

    return size;
}
