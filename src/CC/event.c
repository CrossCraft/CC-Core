#include <CC/event.h>
#include <CC/player.h>
#include <string.h>
#include <stdio.h>

// "In" inside of this context is when the game pushes to the server library
// "Out" inside of this context is when the client will poll from the server library
static EventQueue CC_Event_QueueIn;
static EventQueue CC_Event_QueueOut;

void CC_Event_Init(void) {
    memset(&CC_Event_QueueIn, 0, sizeof(EventQueue));
    memset(&CC_Event_QueueOut, 0, sizeof(EventQueue));
}

void CC_Event_Term(void) {
    // Nothing to do here
}

void CC_Event_Push(CC_Event* event) {
    if( (CC_Event_QueueIn.tail + 1) % CC_MAX_EVENTS == CC_Event_QueueIn.head ) {
        // Queue is full
        return;
    }

    CC_Event_QueueIn.events[CC_Event_QueueIn.tail] = *event;
    CC_Event_QueueIn.tail = (CC_Event_QueueIn.tail + 1) % CC_MAX_EVENTS;
}

CC_Event* CC_Event_Poll(void){
    if(CC_Event_QueueOut.head == CC_Event_QueueOut.tail) {
        // Queue is empty
        return NULL;
    }

    CC_Event* event = &CC_Event_QueueOut.events[CC_Event_QueueOut.head];
    CC_Event_QueueOut.head = (CC_Event_QueueOut.head + 1) % CC_MAX_EVENTS;
    return event;
}

CC_Event* CC_Event_Poll_Internal(void){
    if(CC_Event_QueueIn.head == CC_Event_QueueIn.tail) {
        // Queue is empty
        return NULL;
    }

    CC_Event* event = &CC_Event_QueueIn.events[CC_Event_QueueIn.head];
    CC_Event_QueueIn.head = (CC_Event_QueueIn.head + 1) % CC_MAX_EVENTS;
    return event;
}

void CC_EventQueue_Transfer(CC_Event* in, EventQueue* out) {
    if( (out->tail + 1) % CC_MAX_EVENTS == out->head ) {
        // Queue is full
        return;
    }

    out->events[out->tail] = *in;
    out->tail = (out->tail + 1) % CC_MAX_EVENTS;
}

#define EVENT_DEBUG 0

void CC_Event_Handle_InBound_Client(void) {
    CC_Event* event;

    while ((event = CC_Event_Poll_Internal()) != NULL) {
        switch (event->type) {
            case CC_EVENT_PLAYER_UPDATE: {
                #if EVENT_DEBUG
                printf("EVENT DEBUG: Player Update Event:\n");
                printf("Player ID: %d\n", event->player_update.playerID);
                printf("X Y Z: %f %f %f\n", event->player_update.x, event->player_update.y, event->player_update.z);
                printf("Pitch Yaw: %f %f\n", event->player_update.pitch, event->player_update.yaw);
                printf("On Ground: %d\n", event->player_update.on_ground);
                printf("\n");
                #endif

                CC_Player_SetPosition(event->player_update.x, event->player_update.y, event->player_update.z);
                CC_Player_SetRotation(event->player_update.pitch, event->player_update.yaw);
                CC_Player_SetOnGround(event->player_update.on_ground);
                break;
            }
        }
    }
}

void CC_Event_Push_PlayerUpdate(uint8_t playerID, float x, float y, float z, float pitch, float yaw, bool on_ground) {
    CC_Event event;

    event.type = CC_EVENT_PLAYER_UPDATE;
    event.player_update.playerID = playerID;
    event.player_update.x = x;
    event.player_update.y = y;
    event.player_update.z = z;
    event.player_update.pitch = pitch;
    event.player_update.yaw = yaw;
    event.player_update.on_ground = on_ground;

    CC_Event_Push(&event);
}
