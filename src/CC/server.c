#include <CC/server.h>
#include <CC/eventpackets.h>
#include <CC/player.h>
#include <CC/world.h>
#include <stdio.h>

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

void CC_Server_Handle_PlayerDigging(void* loop, EventPacket* packet) {
    block_t block;
    CC_World_TryGetBlock(packet->data.player_digging_cs.x, packet->data.player_digging_cs.y, packet->data.player_digging_cs.z, &block);

    if(packet->data.player_digging_cs.status == BREAK_FINISH) {
        CC_World_SetBlock(packet->data.player_digging_cs.x, packet->data.player_digging_cs.y, packet->data.player_digging_cs.z, 0);

	if(block == BLK_Flower1) {
		CC_World_RemoveLight(packet->data.player_place_cs.x, packet->data.player_place_cs.y, packet->data.player_place_cs.z);
	} else if(block != BLK_Air && block != BLK_Water && block != BLK_Glass && block != BLK_Leaves && block != BLK_Mushroom2 && block != BLK_Mushroom1 && block!= BLK_Flower2) {
		CC_World_RemoveLight(packet->data.player_place_cs.x, packet->data.player_place_cs.y, packet->data.player_place_cs.z);
	}

        EventPacket block_change = CC_EventPacket_Create_BlockChange(packet->data.player_digging_cs.x, packet->data.player_digging_cs.y, packet->data.player_digging_cs.z, 0);
        CC_EventLoop_PushPacketOutbound(loop, &block_change);
    }
}

void CC_Server_Handle_PlayerBlockPlacement(void* loop, EventPacket* packet) {
    CC_World_SetBlock(packet->data.player_place_cs.x, packet->data.player_place_cs.y, packet->data.player_place_cs.z, packet->data.player_place_cs.item_id);

    block_t block = packet->data.player_place_cs.item_id;
    if(block == BLK_Flower1) {
	CC_World_SetLight(packet->data.player_place_cs.x, packet->data.player_place_cs.y, packet->data.player_place_cs.z, 15);
    } else if(block != BLK_Air && block != BLK_Water && block != BLK_Glass && block != BLK_Leaves && block != BLK_Mushroom2 && block != BLK_Mushroom1 && block!= BLK_Flower2) {
	CC_World_RemoveLight(packet->data.player_place_cs.x, packet->data.player_place_cs.y, packet->data.player_place_cs.z);
    }

    EventPacket block_change = CC_EventPacket_Create_BlockChange(packet->data.player_place_cs.x, packet->data.player_place_cs.y, packet->data.player_place_cs.z, packet->data.player_place_cs.item_id);
    CC_EventLoop_PushPacketOutbound(loop, &block_change);
}

void CC_Server_Register_Handlers(EventLoop* event_loop) {
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_HANDSHAKE, CC_Server_Handle_Handshake);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_LOGIN, CC_Server_Handle_LoginClient);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK, CC_Server_Handle_PlayerPositionAndLook);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_RESPAWN, CC_Server_Handle_PlayerRespawn);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_PLAYER_DIGGING, CC_Server_Handle_PlayerDigging);
    CC_EventLoop_RegisterHandler(event_loop, CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT, CC_Server_Handle_PlayerBlockPlacement);
}
