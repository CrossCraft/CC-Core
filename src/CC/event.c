#include <CC/event.h>
#include <CC/player.h>
#include <CC/world.h>
#include <CC/item.h>
#include <CC/entity.h>
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
                CC_Player_SetPosition(event->data.player_update.x, event->data.player_update.y, event->data.player_update.z);
                CC_Player_SetRotation(event->data.player_update.pitch, event->data.player_update.yaw);
                CC_Player_SetOnGround(event->data.player_update.on_ground);
                break;
            }

            case CC_EVENT_SPAWN_ITEM: {
                #if EVENT_DEBUG
                printf("EVENT DEBUG: Spawn Item Event:\n");
                #endif
                break;
            }

            case CC_EVENT_SET_PLAYER_HEALTH: {
                #if EVENT_DEBUG
                printf("EVENT DEBUG: Set Player Health Event:\n");
                #endif
                break;
            }

            case CC_EVENT_DESTROY_ENTITY: {
                #if EVENT_DEBUG
                printf("EVENT DEBUG: Destroy Entity Event:\n");
                #endif

                CC_Entity_Destroy(event->data.destroy_entity.eid);
                CC_EventQueue_Transfer(event, &CC_Event_QueueOut);
                break;
            }

            case CC_EVENT_SET_BLOCK: {
                #if EVENT_DEBUG
                printf("EVENT DEBUG: Set Block Event:\n");
                printf("X Y Z: %d %d %d\n", event->data.set_block.x, event->data.set_block.y, event->data.set_block.z);
                printf("Mode: %d\n", event->data.set_block.mode);
                printf("Block: %d\n", event->data.set_block.block);
                printf("\n");
                #endif

                if(event->data.set_block.mode == SET_BLOCK_MODE_BREAK) {
                    // Check to see if we can spawn an item
                    block_t block;
                    CC_World_GetBlock(event->data.set_block.x, event->data.set_block.y, event->data.set_block.z, &block);
                    ItemData item_data = CC_Item_Lookup_Block_Drop(block);

                    if(item_data.count > 0) {
                        CC_Event new_event;
                        new_event.type = CC_EVENT_SPAWN_ITEM;
                        new_event.data.spawn_item.eid = CC_Entity_SpawnItem(event->data.set_block.x, event->data.set_block.y, event->data.set_block.z, item_data.id, item_data.data, item_data.count);
                        new_event.data.spawn_item.x = event->data.set_block.x + 0.5f;
                        new_event.data.spawn_item.y = event->data.set_block.y + 0.5f;
                        new_event.data.spawn_item.z = event->data.set_block.z + 0.5f;
                        new_event.data.spawn_item.item.id = item_data.id;
                        new_event.data.spawn_item.item.data = item_data.data;
                        new_event.data.spawn_item.item.count = item_data.count;
                        CC_EventQueue_Transfer(&new_event, &CC_Event_QueueOut);
                    }
                }

                CC_World_SetBlock(event->data.set_block.x, event->data.set_block.y, event->data.set_block.z, event->data.set_block.block);
                CC_EventQueue_Transfer(event, &CC_Event_QueueOut);
                break;
            }
        }
    }
}

void CC_Event_Push_PlayerUpdate(uint8_t playerID, float x, float y, float z, float pitch, float yaw, bool on_ground) {
    CC_Event event;

    event.type = CC_EVENT_PLAYER_UPDATE;
    event.data.player_update.playerID = playerID;
    event.data.player_update.x = x;
    event.data.player_update.y = y;
    event.data.player_update.z = z;
    event.data.player_update.pitch = pitch;
    event.data.player_update.yaw = yaw;
    event.data.player_update.on_ground = on_ground;

    CC_Event_Push(&event);
}

void CC_Event_Push_SetBlock(uint16_t x, uint16_t y, uint16_t z, uint8_t mode, block_t block) {
    CC_Event event;

    event.type = CC_EVENT_SET_BLOCK;
    event.data.set_block.x = x;
    event.data.set_block.y = y;
    event.data.set_block.z = z;
    event.data.set_block.mode = mode;
    event.data.set_block.block = block;

    CC_Event_Push(&event);
}

void CC_Event_Push_DestroyEntity(uint16_t eid) {
    CC_Event event;

    event.type = CC_EVENT_DESTROY_ENTITY;
    event.data.destroy_entity.eid = eid;

    CC_Event_Push(&event);
}

void CC_Event_Push_SetPlayerHealth(uint16_t health) {
    CC_Event event;

    event.type = CC_EVENT_SET_PLAYER_HEALTH;
    event.data.set_player_health.health = health;

    CC_EventQueue_Transfer(&event, &CC_Event_QueueOut);
}
