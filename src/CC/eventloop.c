#include <CC/eventloop.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <CC/alphaindev.pb-c.h>
#include <stdio.h>
#include <CC/netproto.h>

EventLoop* CC_EventLoop_Init(void) {
    EventLoop* loop = malloc(sizeof(EventLoop));
    if(loop == NULL) {
        return NULL;
    }
    memset(loop, 0, sizeof(EventLoop));

    loop->inbound_events.packets = malloc(sizeof(EventPacket) * 16);
    loop->inbound_events.length = 0;
    loop->inbound_events.capacity = 16;

    loop->outbound_events.packets = malloc(sizeof(EventPacket) * 16);
    loop->outbound_events.length = 0;
    loop->outbound_events.capacity = 16;

    return loop;
}

void CC_EventLoop_Destroy(EventLoop* loop) {
    for(int i = 0; i < 256; i++) {
        if(loop->mapping[i].count > 0) {
            free(loop->mapping[i].handlers);
        }
    }
    free(loop->inbound_events.packets);
    free(loop->outbound_events.packets);
    free(loop);
}

void CC_EventLoop_RegisterHandler(EventLoop* loop, EP_PacketType type, EventHandler handler) {
    EventMapping* emap = &loop->mapping[type];
    if(emap->count == 0) {
        emap->handlers = malloc(sizeof(EventHandler));
    } else {
        emap->handlers = realloc(emap->handlers, sizeof(EventHandler) * (emap->count + 1));
    }
    emap->count++;
    emap->handlers[emap->count - 1] = handler;
}

void CC_EventLoop_PushPacketInbound(EventLoop* loop, EventPacket* packet) {
    if(loop->inbound_events.length == loop->inbound_events.capacity) {
        loop->inbound_events.capacity *= 2;
        loop->inbound_events.packets = realloc(loop->inbound_events.packets, sizeof(EventPacket) * loop->inbound_events.capacity);
    }
    loop->inbound_events.packets[loop->inbound_events.length] = *packet;
    loop->inbound_events.length++;
}

void CC_EventLoop_PushPacketOutbound(EventLoop* loop, EventPacket* packet) {
    if(loop->outbound_events.length == loop->outbound_events.capacity) {
        loop->outbound_events.capacity *= 2;
        loop->outbound_events.packets = realloc(loop->outbound_events.packets, sizeof(EventPacket) * loop->outbound_events.capacity);
    }
    loop->outbound_events.packets[loop->outbound_events.length] = *packet;
    loop->outbound_events.length++;
}

EventPacket CC_EventLoop_DeserializePacket(EventLoop* loop, uint8_t* data, size_t size) {
    GeneralPacket* gp = general_packet__unpack(NULL, size, data);
    EventPacket packet;

    packet.type = (EP_PacketType) gp->packet_type;

    switch(packet.type) {
        case CC_PACKET_TYPE_HANDSHAKE: {
            if(loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_CS);
                packet.data.handshake_cs.username.length = gp->handshake_packet_cs->username->length;
                packet.data.handshake_cs.username.data = malloc(packet.data.handshake_cs.username.length);
                memcpy(packet.data.handshake_cs.username.data, gp->handshake_packet_cs->username->data.data, packet.data.handshake_cs.username.length);
            } else {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_SC);
                packet.data.handshake_sc.response.length = gp->handshake_packet_sc->response->length;
                packet.data.handshake_sc.response.data = malloc(packet.data.handshake_sc.response.length);
                memcpy(packet.data.handshake_sc.response.data, gp->handshake_packet_sc->response->data.data, packet.data.handshake_sc.response.length);
            }
            break;
        }

        default: {
            printf("Unhandled Packet type: %d\n", gp->packet_type);
            break;
        }
    }

    general_packet__free_unpacked(gp, NULL);
    return packet;
}

void CC_EventLoop_FreePacket(EventPacket packet) {
    switch(packet.type) {
        case CC_PACKET_TYPE_HANDSHAKE: {
            free(packet.data.handshake_cs.username.data);
            break;
        }

        default: {
            printf("Unhandled Packet type: %d\n", packet.type);
            break;
        }
    }
}

size_t CC_EventLoop_SerializePacket(EventLoop* loop, EventPacket packet, uint8_t** data) {
    GeneralPacket gp = GENERAL_PACKET__INIT;
    size_t size = 0;

    switch(packet.type) {
        case CC_PACKET_TYPE_HANDSHAKE: {
            if(loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_HANDSHAKE;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_SC;
                HandshakePacketSC hsc = HANDSHAKE_PACKET_SC__INIT;

                NetString response = NET_STRING__INIT;
                response.length = packet.data.handshake_sc.response.length;
                response.data.len = packet.data.handshake_sc.response.length;
                response.data.data = malloc(packet.data.handshake_sc.response.length);
                memcpy(response.data.data, packet.data.handshake_sc.response.data, packet.data.handshake_sc.response.length);
                hsc.response = &response;

                gp.handshake_packet_sc = &hsc;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
                free(hsc.response->data.data);
            } else {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_HANDSHAKE;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_CS;
                HandshakePacketCS hcs = HANDSHAKE_PACKET_CS__INIT;

                NetString response = NET_STRING__INIT;
                response.length = packet.data.handshake_sc.response.length;
                response.data.len = packet.data.handshake_sc.response.length;
                response.data.data = malloc(packet.data.handshake_sc.response.length);
                memcpy(response.data.data, packet.data.handshake_sc.response.data, packet.data.handshake_sc.response.length);
                hcs.username = &response;

                gp.handshake_packet_cs = &hcs;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
                free(hcs.username->data.data);
            }
            break;
        }

        default: {
            printf("Unhandled Packet type: %d\n", packet.type);
            break;
        }
    }

    return size;
}

void CC_EventLoop_Update(EventLoop* loop) {
    // Serialize inbound packets
    size_t input_length;
    uint8_t* data = NULL;
    while((input_length = loop->inbound_bus->read(&data, loop->inbound_context)) != 0) {
        EventPacket packet = CC_EventLoop_DeserializePacket(loop, data, input_length);
        CC_EventLoop_PushPacketInbound(loop, &packet);
        CC_EventLoop_FreePacket(packet);
        free(data);
    }

    // Handle inbound packets
    for(size_t i = 0; i < loop->inbound_events.length; i++) {
        EventPacket packet = loop->inbound_events.packets[i];
        EventMapping* emap = &loop->mapping[packet.type];
        for(size_t j = 0; j < emap->count; j++) {
            emap->handlers[j](loop, &packet);
        }
    }
    loop->inbound_events.length = 0;

    // Handle outbound packets
    for(size_t i = 0; i < loop->outbound_events.length; i++) {
        EventPacket packet = loop->outbound_events.packets[i];
        size_t output_length;
        uint8_t* out_data = NULL;
        output_length = CC_EventLoop_SerializePacket(loop, packet, &out_data);
        loop->outbound_bus->write(out_data, output_length, loop->outbound_context);
        free(out_data);
        CC_EventLoop_FreePacket(packet);
    }
    loop->outbound_events.length = 0;
}

void CC_EventLoop_SetInboundBus(EventLoop* loop, DataBus* bus, void* context) {
    loop->inbound_bus = bus;
    loop->inbound_context = context;
}

void CC_EventLoop_SetOutboundBus(EventLoop* loop, DataBus* bus, void* context) {
    loop->outbound_bus = bus;
    loop->outbound_context = context;
}

void CC_EventLoop_SetServer(EventLoop* loop, bool server) {
    loop->server = server;
}
