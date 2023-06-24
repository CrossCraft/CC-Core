#pragma once
#include <CC/type.h>

#if __cplusplus
extern "C" {
#endif

#define PLAYER_SELF -1

void CC_Player_Init(void);
void CC_Player_Term(void);

void CC_Player_Update(void);

void CC_Player_GetPosition(float* x, float* y, float* z);
void CC_Player_SetPosition(float x, float y, float z);

void CC_Player_GetRotation(float* x, float* y);
void CC_Player_SetRotation(float x, float y);

void CC_Player_GetVelocity(float* x, float* y, float* z);
void CC_Player_SetVelocity(float x, float y, float z);

void CC_Player_GetOnGround(bool* on_ground);
void CC_Player_SetOnGround(bool on_ground);

void CC_Player_Respawn(void);

PlayerData* CC_Player_GetData(void);

#if __cplusplus
}
#endif
