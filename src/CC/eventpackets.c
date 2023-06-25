#include <CC/eventpackets.h>
#include <string.h>
#include <stdlib.h>
#include <CC/player.h>

EventPacket CC_EventPacket_Create_Handshake(const char* username, bool server) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_HANDSHAKE;

    if(server) {
        packet.data.handshake_sc.response.length = strlen("-");
        packet.data.handshake_sc.response.data = malloc(packet.data.handshake_sc.response.length);
        memcpy(packet.data.handshake_sc.response.data, "-", packet.data.handshake_sc.response.length);
        return packet;
    }

    packet.data.handshake_cs.username.length = strlen(username);
    packet.data.handshake_cs.username.data = malloc(packet.data.handshake_cs.username.length);
    memcpy(packet.data.handshake_cs.username.data, username, packet.data.handshake_cs.username.length);
    return packet;
}

EventPacket CC_EventPacket_Create_LoginClient(const char* username, const char* password) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_LOGIN;
    EP_LoginPacketCS* login = &packet.data.login_cs;

    login->username.length = strlen(username);
    login->username.data = malloc(login->username.length);
    memcpy(login->username.data, username, login->username.length);

    login->password.length = strlen(password);
    login->password.data = malloc(login->password.length);
    memcpy(login->password.data, password, login->password.length);

    login->protocol_version = 6;
    login->seed = 0;
    login->dimension = 0;

    return packet;
}

EventPacket CC_EventPacket_Create_LoginServer(void) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_LOGIN;

    for(int i = 0; i < 8; i++) {
        packet.data.login_sc.zeroes[i] = 0;
    }

    return packet;
}

EventPacket CC_EventPacket_Create_SpawnPosition(int x, int y, int z) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_SPAWN_POSITION;
    packet.data.spawn_position.x = x;
    packet.data.spawn_position.y = y;
    packet.data.spawn_position.z = z;
    return packet;
}

EventPacket CC_EventPacket_Create_PlayerPositionAndLookClient(float x, float y, float z, float yaw, float pitch, bool on_ground) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK;
    packet.data.player_position_and_look_cs.x = x;
    packet.data.player_position_and_look_cs.y = y;
    packet.data.player_position_and_look_cs.z = z;
    packet.data.player_position_and_look_cs.yaw = yaw;
    packet.data.player_position_and_look_cs.pitch = pitch;
    packet.data.player_position_and_look_cs.on_ground = on_ground;
    return packet;
}

EventPacket CC_EventPacket_Create_PlayerRespawn(void) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_RESPAWN;
    return packet;
}

EventPacket CC_EventPacket_Create_BlockChange(int x, int y, int z, int block_id) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_BLOCK_CHANGE;
    packet.data.block_change_sc.x = x;
    packet.data.block_change_sc.y = y;
    packet.data.block_change_sc.z = z;
    packet.data.block_change_sc.type = block_id;
    return packet;
}

EventPacket CC_EventPacket_Create_PlayerPositionAndLookServer(float x, float y, float z, float yaw, float pitch, bool on_ground) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK;
    packet.data.player_position_and_look_sc.x = x;
    packet.data.player_position_and_look_sc.y = y;
    packet.data.player_position_and_look_sc.z = z;
    packet.data.player_position_and_look_sc.yaw = yaw;
    packet.data.player_position_and_look_sc.pitch = pitch;
    packet.data.player_position_and_look_sc.on_ground = on_ground;
    return packet;
}

EventPacket CC_EventPacket_Create_UpdateHealthServer(void) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_UPDATE_HEALTH;
    packet.data.update_health.health = CC_Player_GetData()->health;
    return packet;
}

EventPacket CC_EventPacket_Create_PlayerDigging(int x, int y, int z, int face, int status) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_PLAYER_DIGGING;
    packet.data.player_digging_cs.x = x;
    packet.data.player_digging_cs.y = y;
    packet.data.player_digging_cs.z = z;
    packet.data.player_digging_cs.face = face;
    packet.data.player_digging_cs.status = status;
    return packet;
}

EventPacket CC_EventPacket_Create_PlayerPlace(int x, int y, int z, int face, int item_id) {
    EventPacket packet;
    packet.type = CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT;
    packet.data.player_place_cs.x = x;
    packet.data.player_place_cs.y = y;
    packet.data.player_place_cs.z = z;
    packet.data.player_place_cs.face = face;
    packet.data.player_place_cs.item_id = item_id;
    return packet;
}
