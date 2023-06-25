#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Block type
typedef uint8_t block_t;
typedef uint16_t item_t;

// Item data structure
typedef struct {
    int16_t item_id;
    int8_t count;
    int16_t damage;
} Item;

// Slot Data Structure
typedef struct {
    uint8_t slot;
    Item data;
} SlotData;

// World data structure
typedef struct {
    size_t x, y, z;
    block_t* blocks;
    uint8_t* lightmap;
} WorldData;

// Player data structure
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float pitch, yaw;
    bool on_ground;

    int8_t health;
    int16_t air;
} PlayerData;

typedef enum {
    CC_OBJECT_TYPE_BOAT,
    CC_OBJECT_TYPE_MINECART,
    CC_OBJECT_TYPE_MINECART_CHEST,
    CC_OBJECT_TYPE_MINECART_FURNACE,
    CC_OBJECT_TYPE_PRIMED_TNT,
    CC_OBJECT_TYPE_ARROW,
    CC_OBJECT_TYPE_SNOWBALL,
    CC_OBJECT_TYPE_FISHING_BOB
} ObjectType;

// Constants for block types
extern const block_t BLK_Air;
extern const block_t BLK_Stone;
extern const block_t BLK_Dirt;
extern const block_t BLK_Grass;
extern const block_t BLK_Cobblestone;
extern const block_t BLK_Wood;
extern const block_t BLK_Sapling;
extern const block_t BLK_Bedrock;
extern const block_t BLK_Water;
extern const block_t BLK_Still_Water;
extern const block_t BLK_Lava;
extern const block_t BLK_Still_Lava;
extern const block_t BLK_Sand;
extern const block_t BLK_Gravel;
extern const block_t BLK_Gold_Ore;
extern const block_t BLK_Iron_Ore;
extern const block_t BLK_Coal_Ore;
extern const block_t BLK_Logs;
extern const block_t BLK_Leaves;
extern const block_t BLK_Sponge;
extern const block_t BLK_Glass;
extern const block_t BLK_Flower1;
extern const block_t BLK_Flower2;
extern const block_t BLK_Mushroom1;
extern const block_t BLK_Mushroom2;
extern const block_t BLK_Gold;
extern const block_t BLK_Iron;
extern const block_t BLK_Double_Slab;
extern const block_t BLK_Slab;
extern const block_t BLK_Brick;
extern const block_t BLK_TNT;
extern const block_t BLK_Bookshelf;
extern const block_t BLK_Mossy_Rocks;
extern const block_t BLK_Obsidian;
extern const block_t BLK_Torch;
extern const block_t BLK_Fire;
extern const block_t BLK_Water_Spawner;
extern const block_t BLK_Lava_Spawner;
extern const block_t BLK_Chest;
extern const block_t BLK_Gear;
extern const block_t BLK_Diamond_Ore;
extern const block_t BLK_Diamond;
extern const block_t BLK_Crafting_Table;
extern const block_t BLK_Wheat;
extern const block_t BLK_Farmland;
extern const block_t BLK_Furnace;
extern const block_t BLK_Furnace_Lit;

extern const item_t ITM_Iron_Shovel;
extern const item_t ITM_Iron_Pickaxe;
extern const item_t ITM_Iron_Axe;
extern const item_t ITM_Flint_and_Steel;
extern const item_t ITM_Apple;
extern const item_t ITM_Bow;
extern const item_t ITM_Arrow;
extern const item_t ITM_Coal;
extern const item_t ITM_Diamond;
extern const item_t ITM_Iron_Ingot;
extern const item_t ITM_Gold_Ingot;
extern const item_t ITM_Iron_Sword;
extern const item_t ITM_Wooden_Sword;
extern const item_t ITM_Wooden_Shovel;
extern const item_t ITM_Wooden_Pickaxe;
extern const item_t ITM_Wooden_Axe;
extern const item_t ITM_Stone_Sword;
extern const item_t ITM_Stone_Shovel;
extern const item_t ITM_Stone_Pickaxe;
extern const item_t ITM_Stone_Axe;
extern const item_t ITM_Diamond_Sword;
extern const item_t ITM_Diamond_Shovel;
extern const item_t ITM_Diamond_Pickaxe;
extern const item_t ITM_Diamond_Axe;
extern const item_t ITM_Stick;
extern const item_t ITM_Bowl;
extern const item_t ITM_Mushroom_Stew;
extern const item_t ITM_Golden_Sword;
extern const item_t ITM_Golden_Shovel;
extern const item_t ITM_Golden_Pickaxe;
extern const item_t ITM_Golden_Axe;
extern const item_t ITM_String;
extern const item_t ITM_Feather;
extern const item_t ITM_Gunpowder;
extern const item_t ITM_Wooden_Hoe;
extern const item_t ITM_Stone_Hoe;
extern const item_t ITM_Iron_Hoe;
extern const item_t ITM_Diamond_Hoe;
extern const item_t ITM_Golden_Hoe;
extern const item_t ITM_Seeds;
extern const item_t ITM_Wheat;
extern const item_t ITM_Bread;
extern const item_t ITM_Leather_Cap;
extern const item_t ITM_Leather_Tunic;
extern const item_t ITM_Leather_Pants;
extern const item_t ITM_Leather_Boots;
extern const item_t ITM_Chain_Helmet;
extern const item_t ITM_Chain_Chestplate;
extern const item_t ITM_Chain_Leggings;
extern const item_t ITM_Chain_Boots;
extern const item_t ITM_Iron_Helmet;
extern const item_t ITM_Iron_Chestplate;
extern const item_t ITM_Iron_Leggings;
extern const item_t ITM_Iron_Boots;
extern const item_t ITM_Diamond_Helmet;
extern const item_t ITM_Diamond_Chestplate;
extern const item_t ITM_Diamond_Leggings;
extern const item_t ITM_Diamond_Boots;
extern const item_t ITM_Golden_Helmet;
extern const item_t ITM_Golden_Chestplate;
extern const item_t ITM_Golden_Leggings;
extern const item_t ITM_Golden_Boots;
extern const item_t ITM_Flint;
extern const item_t ITM_Raw_Porkchop;
extern const item_t ITM_Cooked_Porkchop;
extern const item_t ITM_Painting;

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

/**
 * @brief Block face direction
 */
typedef enum {
    CC_FACE_NEGATIVE_Y = 0,
    CC_FACE_POSITIVE_Y = 1,
    CC_FACE_NEGATIVE_Z = 2,
    CC_FACE_POSITIVE_Z = 3,
    CC_FACE_NEGATIVE_X = 4,
    CC_FACE_POSITIVE_X = 5
} FaceDirection;

/**
 * @brief Block breaking status
 *
 * @details
 * 0: Break start on a block
 * 1: Break progress
 * 2: Break aborted
 * 3: Break finished
 */
typedef enum {
    BREAK_START = 0,
    BREAK_DIG = 1,
    BREAK_ABORT = 2,
    BREAK_FINISH = 3,
    BREAK_DROP = 4,
    BREAK_SHOOT = 5
} BlockBreakingStatus;

/**
 *
 */
typedef enum {
    CC_MOB_TYPE_CREEPER,
    CC_MOB_TYPE_SKELETON,
    CC_MOB_TYPE_SPIDER,
    CC_MOB_TYPE_GIANT,
    CC_MOB_TYPE_ZOMBIE,
    CC_MOB_TYPE_SLIME,
    CC_MOB_TYPE_GHAST,
    CC_MOB_TYPE_ZOMBIE_PIGMAN,
    CC_MOB_TYPE_PIG,
    CC_MOB_TYPE_SHEEP,
    CC_MOB_TYPE_COW,
    CC_MOB_TYPE_CHICKEN,
} MobTypes;

typedef enum {
    CC_ENTITY_STATUS_DAMAGE = 2,
    CC_ENTITY_STATUS_DEATH_ANIMATION = 3,
    CC_ENTITY_STATUS_PRIMED_TNT = 4,
    CC_ENTITY_STATUS_NOT_PRIMED_TNT = 5
} EntityStatuses;

#ifdef __cplusplus
}
#endif
