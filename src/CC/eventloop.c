#include <CC/eventloop.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <CC/alphaindev.pb-c.h>
#include <stdio.h>
#include <CC/netproto.h>

static EventMapping CC_EventLoop_Mapping[256];

static EventList CC_EventLoop_Inbound;
static EventList CC_EventLoop_Outbound;

//static DataBus CC_EventDataBus;

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

void CC_EventLoop_RegisterHandler(EP_PacketType type, EventHandler handler) {
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

void copyNetString(net_string* dest, NetString* src) {
    dest->length = src->length;
    dest->data = malloc(dest->length);
    memcpy(dest->data, src->data.data, dest->length);
}

EventPacket* CC_EventLoop_DeserializePacket(uint8_t* data, size_t size) {
    EventPacket* packet = malloc(sizeof(EventPacket));
    GeneralPacket* gp = general_packet__unpack(NULL, size, data);

    switch(gp->packet_content_case) {
        case GENERAL_PACKET__PACKET_CONTENT__NOT_SET:
            fprintf(stderr, "CC_EventLoop_DeserializePacket: Packet content not set\n");
            break;
        case GENERAL_PACKET__PACKET_CONTENT_KEEP_ALIVE_PACKET:
            packet->type = CC_PACKET_TYPE_PING;
            packet->data.keep_alive.id = gp->keep_alive_packet->id;
            break;
        case GENERAL_PACKET__PACKET_CONTENT_LOGIN_PACKET_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_LOGIN_PACKET_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_CS:
            packet->type = CC_PACKET_TYPE_HANDSHAKE;
            copyNetString(&packet->data.handshake_cs.username, gp->handshake_packet_cs->username);
            break;
        case GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_CHAT_MESSAGE_PACKET:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_TIME_UPDATE_PACKET_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_EQUIPMENT_PACKET_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_SPAWN_POSITION_PACKET_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_USE_ENTITY_PACKET_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_UPDATE_HEALTH_PACKET_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_RESPAWN_DATA_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_PLAYER_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_PLAYER_POSITION_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_PLAYER_LOOK_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_PLAYER_POSITION_AND_LOOK_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_PLAYER_POSITION_AND_LOOK_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_PLAYER_DIGGING_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_PLAYER_PLACE_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_HELD_ITEM_CHANGE_CS:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ADD_TO_INVENTORY_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_ANIMATION:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_SPAWN_NAMED_ENTITY_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_SPAWN_DROPPED_ITEM_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_COLLECT_ITEM_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_SPAWN_OBJECT_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_SPAWN_MOB_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_VELOCITY_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_DESTROY_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_RELATIVE_MOVE_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_LOOK_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_LOOK_AND_RELATIVE_MOVE_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_TELEPORT_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ENTITY_STATUS_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_ATTACH_ENTITY_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_LEVEL_INITIALIZE_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_LEVEL_DATA_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_LEVEL_FINALIZE_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_MULTI_BLOCK_CHANGE_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_BLOCK_CHANGE_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_TILE_ENTITY_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_EXPLOSION_SC:
            break;
        case GENERAL_PACKET__PACKET_CONTENT_KICK_SC:
            break;
        case _GENERAL_PACKET__PACKET_CONTENT__CASE_IS_INT_SIZE:
            break;
    }
    return packet;
}

void CC_EventLoop_FreePacket(EventPacket* packet) {
    //TODO: Hande freeing of packet data
    free(packet);
}

size_t CC_EventLoop_SerializePacket(EventPacket* packet, uint8_t** data);

void CC_EventLoop_Update(void) {
}

void CC_EventLoop_SetBus(DataBus* bus);
