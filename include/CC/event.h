#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <CC/type.h>

#if __cplusplus
extern "C" {
#endif

#define CC_MAX_EVENTS 256

typedef struct {
    uint8_t playerID;

    float x, y, z;
    float pitch, yaw;
    bool on_ground;
} PlayerUpdateEvent;

typedef enum {
    SET_BLOCK_MODE_BREAK = 0x00,
    SET_BLOCK_MODE_PLACE = 0x01,
} SetBlockMode;

typedef struct {
    uint16_t x, y, z;
    SetBlockMode mode;
    block_t block;
} SetBlockEvent;

typedef struct {
    uint16_t eid;
    float x, y, z;
    float vx, vy, vz;
    Item item;
} SpawnItemEvent;

typedef struct {
    uint16_t eid;
} DestroyEntityEvent;

typedef struct {
    int16_t health;
} SetPlayerHealthEvent;

typedef struct {
    uint16_t eid;
    float x, y, z;
    float vx, vy, vz;
    float pitch, yaw;
} EntityTeleportEvent;

typedef struct {
    void* data;
} PlayerRespawnEvent;

typedef enum {
    CC_EVENT_PLAYER_UPDATE, // This event can go both ways
    CC_EVENT_SET_BLOCK, // This event can go both ways
    CC_EVENT_SPAWN_ITEM, // This event only goes to client
    CC_EVENT_DESTROY_ENTITY, // This event can go both ways
    CC_EVENT_SET_PLAYER_HEALTH, // This event only goes to client
    CC_EVENT_ENTITY_TELEPORT, // This event only goes to client
    CC_EVENT_PLAYER_RESPAWN, // This event only goes to core
} EventType;

typedef struct {
    EventType type;
    union {
        PlayerUpdateEvent player_update;
        SetBlockEvent set_block;
        SpawnItemEvent spawn_item;
        DestroyEntityEvent destroy_entity;
        SetPlayerHealthEvent set_player_health;
        EntityTeleportEvent entity_teleport;
        PlayerRespawnEvent player_respawn;
    } data;
} CC_Event;

typedef struct {
    CC_Event events[256];
    size_t head;
    size_t tail;
} EventQueue;

void CC_Event_Init(void);
void CC_Event_Term(void);

void CC_Event_Push(CC_Event* event);

void CC_Event_Handle_InBound_Client(void);
CC_Event* CC_Event_Poll(void);

void CC_Event_Push_PlayerUpdate(uint8_t playerID, float x, float y, float z, float pitch, float yaw, bool on_ground);
void CC_Event_Push_SetBlock(uint16_t x, uint16_t y, uint16_t z, uint8_t mode, block_t block);
void CC_Event_Push_DestroyEntity(uint16_t eid);
void CC_Event_Push_SetPlayerHealth(uint16_t health);
void CC_Event_Push_EntityTeleport(uint16_t eid, float x, float y, float z, float vx, float vy, float vz, float pitch, float yaw);
void CC_Event_Push_SpawnItem(Item item_data, float x, float y, float z, float vx, float vy, float vz);
void CC_Event_Push_PlayerRespawn(void);

#if __cplusplus
}
#endif
