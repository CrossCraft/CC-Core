#include <CC/core.h>
#include <CC/entity.h>
#include <CC/eventloop.h>
#include <stdio.h>
#include <string.h>
#include <CC/eventpackets.h>

static EventLoop * CC_GLOBAL_event_loop = NULL;

EventLoop* CC_Core_GetEventLoop(void) {
    return CC_GLOBAL_event_loop;
}

void CC_Core_Init(void) {
    CC_World_Init(256, 64, 256);

    if(!CC_World_Load()) {
        CC_World_Generate();
    }

    CC_Event_Init();

    CC_Player_Init();
}

void CC_Server_Handle_PlayerPositionAndLook(void* loop, EventPacket* packet) {
    (void)loop; // DISCARD LOOP

    PlayerData* data = CC_Player_GetData();
    data->x = packet->data.player_position_and_look_cs.x;
    data->y = packet->data.player_position_and_look_cs.y;
    data->z = packet->data.player_position_and_look_cs.z;

    data->pitch = packet->data.player_position_and_look_cs.pitch;
    data->yaw = packet->data.player_position_and_look_cs.yaw;

    data->on_ground = packet->data.player_position_and_look_cs.on_ground;
}

void CC_Server_Handle_PlayerRespawn(void* loop, EventPacket* packet) {
    (void)packet; // DISCARD LOOP

    CC_Player_Respawn();
    EventPacket spawnPosition = CC_EventPacket_Create_SpawnPosition(128, 40, 128);
    CC_EventLoop_PushPacketOutbound(loop, &spawnPosition);

    EventPacket playerPositionAndLook = CC_EventPacket_Create_PlayerPositionAndLookServer(128, 40, 128, 0, 0, false);
    CC_EventLoop_PushPacketOutbound(loop, &playerPositionAndLook);

    EventPacket updateHealth = CC_EventPacket_Create_UpdateHealthServer();
    CC_EventLoop_PushPacketOutbound(loop, &updateHealth);
}

void CC_Server_Handle_LoginClient(void* loop, EventPacket* packet) {
    printf("Client Logging In Username: %s\n", packet->data.login_cs.username.data);
    printf("Client Logging In Password: %s\n", packet->data.login_cs.password.data);

    EventPacket packet_response = CC_EventPacket_Create_LoginServer();

    EventLoop* eloop = (EventLoop*)loop;
    CC_EventLoop_PushPacketOutbound(eloop, &packet_response);

    EventPacket spawnPosition = CC_EventPacket_Create_SpawnPosition(128, 40, 128);
    CC_EventLoop_PushPacketOutbound(eloop, &spawnPosition);

    EventPacket playerPositionAndLook = CC_EventPacket_Create_PlayerPositionAndLookServer(128, 40, 128, 0, 0, false);
    CC_EventLoop_PushPacketOutbound(eloop, &playerPositionAndLook);

    EventPacket updateHealth = CC_EventPacket_Create_UpdateHealthServer();
    CC_EventLoop_PushPacketOutbound(eloop, &updateHealth);
}

void CC_Server_Handle_Handshake(void* loop, EventPacket* packet) {
    printf("Client Connecting Username: %s\n", packet->data.handshake_cs.username.data);

    EventPacket packet_response = CC_EventPacket_Create_Handshake("", true);

    EventLoop* eloop = (EventLoop*)loop;
    CC_EventLoop_PushPacketOutbound(eloop, &packet_response);
}

void CC_Core_SetEventLoop(EventLoop * event_loop) {
    CC_GLOBAL_event_loop = event_loop;
    CC_EventLoop_SetServer(event_loop, true);

    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_HANDSHAKE, CC_Server_Handle_Handshake);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_LOGIN, CC_Server_Handle_LoginClient);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK, CC_Server_Handle_PlayerPositionAndLook);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_RESPAWN, CC_Server_Handle_PlayerRespawn);
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
