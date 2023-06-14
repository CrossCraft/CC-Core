#include <CC/eventloop.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <CC/alphaindev.pb-c.h>

static EventMapping CC_EventLoop_Mapping[256];

static EventList CC_EventLoop_Inbound;
static EventList CC_EventLoop_Outbound;

void CC_EventLoop_Init(void) {
    memset(CC_EventLoop_Mapping, 0, sizeof(CC_EventLoop_Mapping));

    CC_EventLoop_Inbound.packets = malloc(sizeof(EventPacket) * 16);
    CC_EventLoop_Inbound.length = 0;
    CC_EventLoop_Inbound.capacity = 16;

    CC_EventLoop_Outbound.packets = malloc(sizeof(EventPacket) * 16);
    CC_EventLoop_Outbound.length = 0;
    CC_EventLoop_Outbound.capacity = 16;
}
void CC_EventLoop_Shutdown(void) {
    for(int i = 0; i < 256; i++) {
        if(CC_EventLoop_Mapping[i].count > 0) {
            free(CC_EventLoop_Mapping[i].handlers);
        }
    }
}

void CC_EventLoop_RegisterHandler(PacketType type, EventHandler handler) {
    EventMapping* emap = &CC_EventLoop_Mapping[type];
    if(emap->count == 0) {
        emap->handlers = malloc(sizeof(EventHandler));
    } else {
        emap->handlers = realloc(emap->handlers, sizeof(EventHandler) * (emap->count + 1));
    }
    emap->count++;
    emap->handlers[emap->count - 1] = handler;
}

void CC_EventLoop_PushPacketInbound(EventPacket* packet) {
    if(CC_EventLoop_Inbound.length == CC_EventLoop_Inbound.capacity) {
        CC_EventLoop_Inbound.capacity *= 2;
        CC_EventLoop_Inbound.packets = realloc(CC_EventLoop_Inbound.packets, sizeof(EventPacket) * CC_EventLoop_Inbound.capacity);
    }
    CC_EventLoop_Inbound.packets[CC_EventLoop_Inbound.length] = *packet;
    CC_EventLoop_Inbound.length++;
}
void CC_EventLoop_PushPacketOutbound(EventPacket* packet) {
    if(CC_EventLoop_Outbound.length == CC_EventLoop_Outbound.capacity) {
        CC_EventLoop_Outbound.capacity *= 2;
        CC_EventLoop_Outbound.packets = realloc(CC_EventLoop_Outbound.packets, sizeof(EventPacket) * CC_EventLoop_Outbound.capacity);
    }
    CC_EventLoop_Outbound.packets[CC_EventLoop_Outbound.length] = *packet;
    CC_EventLoop_Outbound.length++;
}

EventPacket* CC_EventLoop_DeserializePacket(uint8_t* data, size_t size);

void CC_EventLoop_FreePacket(EventPacket* packet) {
    //TODO: Hande freeing of packet data
    free(packet);
}

size_t CC_EventLoop_SerializePacket(EventPacket* packet, uint8_t** data);

void CC_EventLoop_Update(void) {
    for(size_t i = 0; i < CC_EventLoop_Inbound.length; i++) {
        EventPacket* packet = &CC_EventLoop_Inbound.packets[i];
        EventMapping* emap = &CC_EventLoop_Mapping[packet->type];
        for(size_t j = 0; j < emap->count; j++) {
            emap->handlers[j](packet);
        }
        CC_EventLoop_FreePacket(packet);
    }
    CC_EventLoop_Inbound.length = 0;

    for(size_t i = 0; i < CC_EventLoop_Outbound.length; i++) {
        EventPacket *packet = &CC_EventLoop_Outbound.packets[i];
        uint8_t *data;
        size_t size = CC_EventLoop_SerializePacket(packet, &data);
        //TODO: Send data
        free(data);
    }
}
