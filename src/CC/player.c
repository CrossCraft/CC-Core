#include <CC/player.h>
#include <CC/event.h>
#include <stdio.h>
#include <CC/world.h>
#include <CC/eventloop.h>
#include <CC/eventpackets.h>
#include <CC/core.h>

static PlayerData CC_GLOBAL_player_data;
static bool CC_Is_First_Spawn = true;

void CC_Player_Init(void) {
    CC_Player_Respawn();
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

PlayerData* CC_Player_GetData(void) {
    return &CC_GLOBAL_player_data;
}

float last_height_on_ground = 0.0f;
void CC_Player_Check_FallDamage(void) {
    if(CC_GLOBAL_player_data.on_ground) {
        float new_height = CC_GLOBAL_player_data.y;

        float difference = last_height_on_ground - new_height;
        if(difference > 3.0f) {
            block_t foot_block;
            bool success = CC_World_TryGetBlock(CC_GLOBAL_player_data.x, CC_GLOBAL_player_data.y - 1.8f, CC_GLOBAL_player_data.z, &foot_block);

            if(success) {
                if(foot_block != BLK_Water) {
                    CC_GLOBAL_player_data.health -= (difference - 3.0f);

                    if(CC_GLOBAL_player_data.health <= 0) {
                        CC_GLOBAL_player_data.health = 0;
                    }

                    EventPacket updateHealth = CC_EventPacket_Create_UpdateHealthServer();
                    CC_EventLoop_PushPacketOutbound(CC_Core_GetEventLoop(), &updateHealth);
                }
            }
        }

        last_height_on_ground = new_height;
    }
}

void CC_Player_Check_Drown(void) {
    block_t head_block;
    bool success = CC_World_TryGetBlock(CC_GLOBAL_player_data.x, CC_GLOBAL_player_data.y + 1.8f, CC_GLOBAL_player_data.z, &head_block);

    if(success) {
        if(head_block == BLK_Water) {
            CC_GLOBAL_player_data.air -= 1;
            if(CC_GLOBAL_player_data.air <= 0) {
                CC_GLOBAL_player_data.health -= 2;
                CC_GLOBAL_player_data.air = 30;

                if(CC_GLOBAL_player_data.health <= 0) {
                    CC_GLOBAL_player_data.health = 0;
                }

                EventPacket updateHealth = CC_EventPacket_Create_UpdateHealthServer();
                CC_EventLoop_PushPacketOutbound(CC_Core_GetEventLoop(), &updateHealth);
            }
        } else {
            CC_GLOBAL_player_data.air = 300;
        }
    }
}

void CC_Player_Update(void) {
    CC_Player_Check_Drown();

    // Fall Damage Check
    CC_Player_Check_FallDamage();
}

void CC_Player_Respawn(void) {
    CC_GLOBAL_player_data.x = 128.0f;
    CC_GLOBAL_player_data.y = 40.0f;
    CC_GLOBAL_player_data.z = 128.0f;
    CC_GLOBAL_player_data.vx = 0.0f;
    CC_GLOBAL_player_data.vy = 0.0f;
    CC_GLOBAL_player_data.vz = 0.0f;

    CC_GLOBAL_player_data.pitch = 0.0f;
    CC_GLOBAL_player_data.yaw = 0.0f;

    CC_GLOBAL_player_data.on_ground = false;

    CC_GLOBAL_player_data.health = 20;
    CC_GLOBAL_player_data.air = 300;

    if(CC_Is_First_Spawn) {
        CC_Is_First_Spawn = false;
    }
}
