#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Block type
typedef uint8_t block_t;

// World data structure
typedef struct {
    size_t x, y, z;
    block_t* blocks;
} WorldData;

// Player data structure
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float pitch, yaw;
    bool on_ground;
} PlayerData;

// Constants for block types
extern const uint8_t BLK_Air;
extern const uint8_t BLK_Stone;
extern const uint8_t BLK_Dirt;
extern const uint8_t BLK_Grass;
extern const uint8_t BLK_Cobblestone;
extern const uint8_t BLK_Wood;
extern const uint8_t BLK_Sapling;
extern const uint8_t BLK_Bedrock;
extern const uint8_t BLK_Water;
extern const uint8_t BLK_Still_Water;
extern const uint8_t BLK_Lava;
extern const uint8_t BLK_Still_Lava;
extern const uint8_t BLK_Sand;
extern const uint8_t BLK_Gravel;
extern const uint8_t BLK_Gold_Ore;
extern const uint8_t BLK_Iron_Ore;
extern const uint8_t BLK_Coal_Ore;
extern const uint8_t BLK_Logs;
extern const uint8_t BLK_Leaves;
extern const uint8_t BLK_Sponge;
extern const uint8_t BLK_Glass;
extern const uint8_t BLK_Flower1;
extern const uint8_t BLK_Flower2;
extern const uint8_t BLK_Mushroom1;
extern const uint8_t BLK_Mushroom2;
extern const uint8_t BLK_Gold;
extern const uint8_t BLK_Iron;
extern const uint8_t BLK_Double_Slab;
extern const uint8_t BLK_Slab;
extern const uint8_t BLK_Brick;
extern const uint8_t BLK_TNT;
extern const uint8_t BLK_Bookshelf;
extern const uint8_t BLK_Mossy_Rocks;
extern const uint8_t BLK_Obsidian;

// Default lighting color
extern const uint32_t LIGHT_TOP;
extern const uint32_t LIGHT_SIDE_X;
extern const uint32_t LIGHT_SIDE_Z;
extern const uint32_t LIGHT_BOT;

// When dark block color tint is multiplied by 0.6
extern const uint32_t LIGHT_TOP_DARK;
extern const uint32_t LIGHT_SIDE_X_DARK;
extern const uint32_t LIGHT_SIDE_Z_DARK;
extern const uint32_t LIGHT_BOT_DARK;

#ifdef __cplusplus
}
#endif
