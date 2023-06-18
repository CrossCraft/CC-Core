#pragma once
#include <CC/eventloop.h>

#ifdef __cplusplus
extern "C" {
#endif

EventPacket CC_EventPacket_Create_Handshake(const char* username, bool server);
EventPacket CC_EventPacket_Create_LoginClient(const char* username, const char* password);
EventPacket CC_EventPacket_Create_LoginServer(void);

EventPacket CC_EventPacket_Create_SpawnPosition(int x, int y, int z);
EventPacket CC_EventPacket_Create_PlayerPositionAndLookServer(float x, float y, float z, float yaw, float pitch, bool on_ground);
EventPacket CC_EventPacket_Create_UpdateHealthServer(void);

#ifdef __cplusplus
}
#endif