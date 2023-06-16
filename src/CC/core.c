#include <CC/core.h>
#include <CC/entity.h>
#include <CC/eventloop.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static EventLoop * CC_GLOBAL_event_loop = NULL;

void CC_Core_Init(void) {
    CC_World_Init(256, 64, 256);

    if(!CC_World_Load()) {
        CC_World_Generate();
    }

    CC_Event_Init();

    CC_Player_Init();
}

void CC_Server_Handle_Handshake(void* loop, EventPacket* packet) {
    printf("Handshake packet received\n");
    printf("Client Username: %s\n", packet->data.handshake_cs.username.data);

    EventPacket* packet_response = malloc(sizeof(EventPacket));
    packet_response->type = CC_PACKET_TYPE_HANDSHAKE;
    packet_response->data.handshake_sc.response.length = strlen("-");
    packet_response->data.handshake_sc.response.data = malloc(packet_response->data.handshake_sc.response.length);
    memcpy(packet_response->data.handshake_sc.response.data, "-", packet_response->data.handshake_sc.response.length);

    EventLoop* eloop = (EventLoop*)loop;
    CC_EventLoop_PushPacketOutbound(eloop, packet_response);
}

void CC_Core_SetEventLoop(EventLoop * event_loop) {
    CC_GLOBAL_event_loop = event_loop;
    CC_EventLoop_SetServer(event_loop, true);

    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_HANDSHAKE, CC_Server_Handle_Handshake);
}

static float CC_GLOBAL_delta_time = 0.0f;

void CC_Core_Update(double delta_time) {
    if(CC_GLOBAL_event_loop != NULL) {
        CC_EventLoop_Update(CC_GLOBAL_event_loop);
    }
    CC_Event_Handle_InBound_Client();

    CC_GLOBAL_delta_time += (float)delta_time;

    CC_Entity_Update(delta_time);

    if(CC_GLOBAL_delta_time >= 0.05f) {
        CC_Player_Update();
        CC_GLOBAL_delta_time = 0.0f;
    }
}

void CC_Core_Term(void) {
    CC_Event_Term();
    CC_Player_Term();
    CC_World_Term();
}
