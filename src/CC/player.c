#include <CC/player.h>

static PlayerData CC_GLOBAL_player_data;

void CC_Player_Init(void) {
    CC_GLOBAL_player_data.x = 0.0f;
    CC_GLOBAL_player_data.y = 0.0f;
    CC_GLOBAL_player_data.z = 0.0f;
    CC_GLOBAL_player_data.vx = 0.0f;
    CC_GLOBAL_player_data.vy = 0.0f;
    CC_GLOBAL_player_data.vz = 0.0f;

    CC_GLOBAL_player_data.pitch = 0.0f;
    CC_GLOBAL_player_data.yaw = 0.0f;

    CC_GLOBAL_player_data.on_ground = false;
}

void CC_Player_Term(void) {
    // Nothing to do here
}

void CC_Player_GetPosition(float* x, float* y, float* z) {
    *x = CC_GLOBAL_player_data.x;
    *y = CC_GLOBAL_player_data.y;
    *z = CC_GLOBAL_player_data.z;
}

void CC_Player_SetPosition(float x, float y, float z) {
    CC_GLOBAL_player_data.x = x;
    CC_GLOBAL_player_data.y = y;
    CC_GLOBAL_player_data.z = z;
}

void CC_Player_GetRotation(float* x, float* y) {
    *x = CC_GLOBAL_player_data.pitch;
    *y = CC_GLOBAL_player_data.yaw;
}

void CC_Player_SetRotation(float x, float y) {
    CC_GLOBAL_player_data.pitch = x;
    CC_GLOBAL_player_data.yaw = y;
}

void CC_Player_GetVelocity(float* x, float* y, float* z) {
    *x = CC_GLOBAL_player_data.vx;
    *y = CC_GLOBAL_player_data.vy;
    *z = CC_GLOBAL_player_data.vz;
}

void CC_Player_SetVelocity(float x, float y, float z) {
    CC_GLOBAL_player_data.vx = x;
    CC_GLOBAL_player_data.vy = y;
    CC_GLOBAL_player_data.vz = z;
}

void CC_Player_GetOnGround(bool* on_ground) {
    *on_ground = CC_GLOBAL_player_data.on_ground;
}

void CC_Player_SetOnGround(bool on_ground) {
    CC_GLOBAL_player_data.on_ground = on_ground;
}
