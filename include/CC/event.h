#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
    CC_EVENT_PLAYER_UPDATE, // This event can go both ways
} EventType;

typedef struct {
    EventType type;
    union {
        PlayerUpdateEvent player_update;
    };
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

#if __cplusplus
}
#endif
