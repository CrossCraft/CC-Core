#pragma once
#include <CC/type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CC_PACKET_TYPE_PING = 0x00,
    CC_PACKET_TYPE_LOGIN = 0x01,
    CC_PACKET_TYPE_HANDSHAKE = 0x02,
    CC_PACKET_TYPE_CHAT_MESSAGE = 0x03,
    CC_PACKET_TYPE_TIME_UPDATE = 0x04,
    CC_PACKET_TYPE_ENTITY_EQUIPMENT = 0x05,
    CC_PACKET_TYPE_SPAWN_POSITION = 0x06,
    CC_PACKET_TYPE_USE_ENTITY = 0x07,
    CC_PACKET_TYPE_UPDATE_HEALTH = 0x08,
    CC_PACKET_TYPE_RESPAWN = 0x09,
    CC_PACKET_TYPE_PLAYER = 0x0A,
    CC_PACKET_TYPE_PLAYER_POSITION = 0x0B,
    CC_PACKET_TYPE_PLAYER_LOOK = 0x0C,
    CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK = 0x0D,
    CC_PACKET_TYPE_PLAYER_DIGGING = 0x0E,
    CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT = 0x0F,
    CC_PACKET_TYPE_HELD_ITEM_CHANGE = 0x10,
    CC_PACKET_TYPE_ADD_TO_INVENTORY = 0x11,
    CC_PACKET_TYPE_ANIMATION = 0x12,
    CC_PACKET_TYPE_NAMED_ENTITY_SPAWN = 0x14,
    CC_PACKET_TYPE_PICKUP_SPAWN = 0x15,
    CC_PACKET_TYPE_COLLECT_ITEM = 0x16,
    CC_PACKET_TYPE_SPAWN_OBJECT_VEHICLE = 0x17,
    CC_PACKET_TYPE_SPAWN_MOB = 0x18,
    CC_PACKET_TYPE_ENTITY_VELOCITY = 0x1C,
    CC_PACKET_TYPE_DESTROY_ENTITY = 0x1D,
    CC_PACKET_TYPE_ENTITY = 0x1E,
    CC_PACKET_TYPE_ENTITY_RELATIVE_MOVE = 0x1F,
    CC_PACKET_TYPE_ENTITY_LOOK = 0x20,
    CC_PACKET_TYPE_ENTITY_LOOK_AND_RELATIVE_MOVE = 0x21,
    CC_PACKET_TYPE_ENTITY_TELEPORT = 0x22,
    CC_PACKET_TYPE_ENTITY_STATUS = 0x26,
    CC_PACKET_TYPE_ATTACH_ENTITY = 0x27,
    CC_PACKET_TYPE_LEVEL_INIT = 0x30,
    CC_PACKET_TYPE_LEVEL_DATA = 0x31,
    CC_PACKET_TYPE_LEVEL_FINALIZE = 0x32,
    CC_PACKET_TYPE_MULTI_BLOCK_CHANGE = 0x34,
    CC_PACKET_TYPE_BLOCK_CHANGE = 0x35,
    CC_PACKET_TYPE_TILE_ENTITY = 0x3B,
    CC_PACKET_TYPE_EXPLOSION = 0x3C,
    CC_PACKET_TYPE_KICK
} PacketType;

typedef struct  {
    uint16_t length;
    uint8_t data[];
} net_string;

typedef struct {
    uint8_t type;
    uint8_t data[];
} Packet;

typedef struct {
    int32_t id;
} PacketPing;

typedef struct {
    int32_t protocol_version; // 4
    net_string username; // 2 + username.length
    net_string password; // 2 + password.length
    uint64_t seed; // 8
    uint8_t dimension; // 1
} PacketLogin; // Total 17 + username.length + password.length

#ifdef __cplusplus
};
#endif
