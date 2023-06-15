#pragma once
#include <CC/type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t length;
    uint8_t *data;
} net_string;

typedef enum {
    CC_PACKET_TYPE_PING = 0x00,
    CC_PACKET_TYPE_LOGIN = 0x01,
    CC_PACKET_TYPE_HANDSHAKE = 0x02,
    CC_PACKET_TYPE_CHAT_MESSAGE = 0x03,
    CC_PACKET_TYPE_TIME_UPDATE = 0x04,
    CC_PACKET_TYPE_ENTITY_EQUIPMENT = 0x05,
    CC_PACKET_TYPE_SPAWN_POSITION = 0x06,
    CC_PACKET_TYPE_USE_ENTITY = 0x07,
    CC_PACKET_TYPE_UPDATE_HEALTH = 0x08,
    CC_PACKET_TYPE_RESPAWN = 0x09,
    CC_PACKET_TYPE_PLAYER = 0x0A,
    CC_PACKET_TYPE_PLAYER_POSITION = 0x0B,
    CC_PACKET_TYPE_PLAYER_LOOK = 0x0C,
    CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK = 0x0D,
    CC_PACKET_TYPE_PLAYER_DIGGING = 0x0E,
    CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT = 0x0F,
    CC_PACKET_TYPE_HELD_ITEM_CHANGE = 0x10,
    CC_PACKET_TYPE_ADD_TO_INVENTORY = 0x11,
    CC_PACKET_TYPE_ANIMATION = 0x12,
    CC_PACKET_TYPE_NAMED_ENTITY_SPAWN = 0x14,
    CC_PACKET_TYPE_SPAWN_DROPPED_ITEM = 0x15,
    CC_PACKET_TYPE_COLLECT_ITEM = 0x16,
    CC_PACKET_TYPE_SPAWN_OBJECT_VEHICLE = 0x17,
    CC_PACKET_TYPE_SPAWN_MOB = 0x18,
    CC_PACKET_TYPE_ENTITY_VELOCITY = 0x1C,
    CC_PACKET_TYPE_DESTROY_ENTITY = 0x1D,
    CC_PACKET_TYPE_ENTITY = 0x1E,
    CC_PACKET_TYPE_ENTITY_RELATIVE_MOVE = 0x1F,
    CC_PACKET_TYPE_ENTITY_LOOK = 0x20,
    CC_PACKET_TYPE_ENTITY_LOOK_AND_RELATIVE_MOVE = 0x21,
    CC_PACKET_TYPE_ENTITY_TELEPORT = 0x22,
    CC_PACKET_TYPE_ENTITY_STATUS = 0x26,
    CC_PACKET_TYPE_ATTACH_ENTITY = 0x27,
    CC_PACKET_TYPE_LEVEL_INIT = 0x30, // CUSTOM PACKET -- BASED ON CLASSIC CLIENT
    CC_PACKET_TYPE_LEVEL_DATA = 0x31, // CUSTOM PACKET -- BASED ON CLASSIC CLIENT
    CC_PACKET_TYPE_LEVEL_FINALIZE = 0x32, // CUSTOM PACKET -- BASED ON CLASSIC CLIENT
    CC_PACKET_TYPE_MULTI_BLOCK_CHANGE = 0x34,
    CC_PACKET_TYPE_BLOCK_CHANGE = 0x35,
    CC_PACKET_TYPE_TILE_ENTITY = 0x3B,
    CC_PACKET_TYPE_EXPLOSION = 0x3C,
    CC_PACKET_TYPE_KICK = 0xFF,
} EP_PacketType;

/**
 * Ping packet
 * @param id The internal ID to respond with
 *
 * This must match the ID sent by the client / server in the ping packet
 */
typedef struct {
    int32_t id;
} EP_KeepAlivePacket;

/**
 * Login packet from Client -> Server
 * @param protocol_version The protocol version of the client
 * @param username The username of the client
 * @param password The password of the client
 * @param seed The seed value -- not used by client
 * @param dimension The dimension used
 */
typedef struct {
    int32_t protocol_version;
    net_string username;
    net_string password;
    uint64_t seed;
    uint8_t dimension;
} EP_LoginPacketCS;

/**
 * Login packet from Server -> Client
 * @param zeroes 8 bytes of zeroes
 * We don't really know what's getting sent back here.
 */
typedef struct {
    uint8_t zeroes[8];
} EP_LoginPacketSC;

/**
 * Handshake packet from Client -> Server
 * @param username The username of the client
 */
typedef struct {
    net_string username;
} EP_HandshakePacketCS;

/**
 * Handshake packet from Server -> Client
 * @param response The response from the server which appears to be 0x2D as ASCII
 */
typedef struct {
    net_string response;
} EP_HandShakePacketSC;

/**
 * Chat message packet from Client <-> Server
 * @param message The message to send
 */
typedef struct {
    net_string message; // Client can send 100 characters, server can send 119 characters
} EP_ChatMessagePacket;

/**
 * Time update packet from Server -> Client
 * @param time The time in ticks
 */
typedef struct {
    int64_t time;
} EP_TimeUpdatePacketSC;

/**
 * Entity equipment packet from Server -> Client
 * @param entity_id The entity ID
 * @param slot The slot to equip
 * @param item_id The item ID
 * @param damage The damage value
 *
 * This is used for player entities only
 * It equips the player with the item in the specified slot
 * This is the four armor slots, plus what is directly in their hand
 */
typedef struct {
    int32_t entity_id;
    int16_t slot;
    int16_t item_id;
    int16_t damage;
} EP_EntityEquipmentPacketSC;

/**
 * Spawn position packet from Server -> Client
 * @param x The X position
 * @param y The Y position
 * @param z The Z position
 *
 * This is used to set the spawn position of the player
 * It is used to determine where the player will respawn
 */
typedef struct {
    int32_t x, y, z;
} EP_SpawnPositionPacketSC;

/**
 * Use entity packet from Client -> Server
 * @param entity_id The entity ID
 * @param target_entity The target entity ID
 * @param left_click Whether or not the left mouse button was clicked
 *
 * This is used to attack entities and use entities like minecarts or boats
 */
typedef struct {
    int32_t entity_id;
    int32_t target_entity;
    uint8_t left_click;
} EP_UseEntityPacketCS;

/**
 * Update health packet from Server -> Client
 * @param health The health of the player
 *
 * This is used to update the health of the player
 */
typedef struct {
    int8_t health;
} EP_UpdateHealthPacketSC;

/**
 * Respawn packet from Server -> Client
 *
 * These packets are actually blank -- they don't contain any data
 */
typedef struct CC_RespawnDataSC EP_RespawnDataSC;

/**
 * Player packet from Client -> Server
 * @param on_ground Whether or not the player is on the ground
 *
 * This is used to update the player's status as being on the ground
 * In reality this packet is what happens when the player is AFK so that the server
 * knows that client is still connected (beyond the keep alive packet)
 */
typedef struct {
    uint8_t on_ground;
} EP_PlayerCS;

/**
 * Player position packet from Client -> Server
 * @param x The X position
 * @param y The Y position
 * @param stance The stance of the player -- this is used to modify the hitbox(?)
 * @param z The Z position
 * @param on_ground Whether or not the player is on the ground
 *
 * This is used to update the player's position on the server
 */
typedef struct {
    double x, y, stance, z;
    uint8_t on_ground;
} EP_PlayerPositionCS;

/**
 * Player look packet from Client -> Server
 * @param yaw The yaw of the player
 * @param pitch The pitch of the player
 * @param on_ground Whether or not the player is on the ground
 *
 * This is used to update the player's look on the server
 */
typedef struct {
    float yaw, pitch;
    uint8_t on_ground;
} EP_PlayerLookCS;

/**
 * Player position and look packet from Client -> Server
 * @param x The X position
 * @param y The Y position
 * @param stance The stance of the player -- this is used to modify the hitbox(?)
 * @param z The Z position
 * @param yaw The yaw of the player
 * @param pitch The pitch of the player
 * @param on_ground Whether or not the player is on the ground
 *
 * This is used to update the player's position and look on the server
 */
typedef struct {
    double x, y, stance, z;
    float yaw, pitch;
    uint8_t on_ground;
} EP_PlayerPositionAndLookCS;

/**
 * Player position and look packet from Server -> Client
 * @param x The X position
 * @param stance The stance of the player -- this is used to modify the hitbox(?)
 * @param y The Y position
 * @param z The Z position
 * @param yaw The yaw of the player
 * @param pitch The pitch of the player
 * @param on_ground Whether or not the player is on the ground
 *
 * This is used to update the player's position and look on the client
 */
typedef struct {
    double x, stance, y, z;
    float yaw, pitch;
    uint8_t on_ground;
} EP_PlayerPositionAndLookSC;

/**
 * Player digging packet from Client -> Server
 * @param status The status of the digging
 * @param x The X position
 * @param y The Y position
 * @param z The Z position
 * @param face The face of the block
 */
typedef struct {
    uint8_t status;
    int32_t x;
    uint8_t y;
    int32_t z;
    uint8_t face;
} EP_PlayerDiggingCS;

/**
 * Player places an item packet from Client -> Server
 * @param x The X position
 *
 */
typedef struct {
    int16_t item_id;

    int32_t x;
    uint8_t y;
    int32_t z;

    uint8_t face;
} EP_PlayerPlaceCS;

/**
 * Holding change packet from Client -> Server
 * @param slot The slot that the player is holding
 */
typedef struct {
    int16_t slot;
} EP_HeldItemChangeCS;

/**
 * Adds an item to the inventory from Server -> Client
 * @param item The item to add
 */
typedef struct {
    Item item;
} EP_AddToInventorySC;

/**
 * Sent whenever an entity needs to be animated from Client <-> Server
 * @param entity_id The entity ID
 * @param animation The animation to play
 */
typedef struct {
    int32_t entity_id;
    uint8_t animation; //TODO: Enum
} EP_EntityAnimation;

/**
 * Sent when a player is spawned from Server -> Client
 * @param entity_id The entity ID
 * @param name The name of the player
 * @param x The X position
 * @param y The Y position
 * @param z The Z position
 * @param yaw The yaw of the player
 * @param pitch The pitch of the player
 * @param item The item that the player is holding
 *
 * This is used to spawn a player on the client
 */
typedef struct {
    int32_t entity_id;
    net_string name;
    int32_t x, y, z;
    int8_t yaw, pitch;
    int16_t item;
} EP_SpawnNamedEntitySC;

/**
 * Sent when an item is dropped from Server -> Client
 * @param entity_id The entity ID
 * @param item The item that was dropped
 * @param x The X position
 * @param y The Y position
 * @param z The Z position
 * @param yaw The yaw of the item
 * @param pitch The pitch of the item
 * @param roll The roll of the item
 *
 * This is used to spawn an item on the client.
 */
typedef struct {
    int32_t entity_id;
    Item item;
    int32_t x, y, z;
    uint8_t yaw, pitch, roll;
} EP_SpawnDroppedItemSC;

/**
 * Sent when an item is picked up (animation only) from Server -> Client
 * @param entity_id The entity ID
 * @param player_id The player ID
 *
 * This is used to animate the player picking up an item
 */
typedef struct {
    int32_t entity_id;
    int32_t player_id;
} EP_CollectItemSC;

/**
 * Sent when an object is spawned from Server -> Client
 * @param entity_id The entity ID
 * @param type The type of the object
 * @param x The X position
 * @param y The Y position
 * @param z The Z position
 */
typedef struct {
    int32_t entity_id;
    uint8_t type;
    int32_t x, y, z;
} EP_SpawnObjectSC;

/**
 * Spawn mob packet from Server -> Client
 *
 */
typedef struct {
    int32_t entity_id;
    uint8_t type;
    int32_t x, y, z;
    uint8_t yaw, pitch;
} EP_SpawnMobSC;

/**
 * Set entity motion packet from Server -> Client
 * @param entity_id The entity ID
 * @param vx The X velocity
 * @param vy The Y velocity
 * @param vz The Z velocity
 *
 * Velocity is believed to be in units of 1/32000 of a block per server tick (200ms);
 * For example, -1343 would move (-1343 / 32000) = -0.04196875 blocks per tick
 */
typedef struct {
   int32_t entity_id;
   int16_t vx, vy, vz;
} EP_EntityVelocitySC;

/**
 * Destroy entity packet from Server -> Client
 */
typedef struct {
    int32_t entity_id;
} EP_EntityDestroySC;

/**
 * Entity packet from Server -> Client
 */
typedef struct {
    int32_t entity_id;
} EP_EntitySC;

/**
 * Entity relative move packet from Server -> Client
 * @param entity_id The entity ID
 * @param dx The X delta
 * @param dy The Y delta
 * @param dz The Z delta
 *
 * This is used to move an entity relative to its current position
 * Value range is -128 to 127
 * Each unit is 1/32 of a block
 * For example, -128 would move -4 blocks
 * This is used for entities such as items and mobs
 */
typedef struct {
    int32_t entity_id;
    int8_t dx, dy, dz;
} EP_EntityRelativeMoveSC;

/**
 * Entity look packet from Server -> Client
 * @param entity_id The entity ID
 * @param yaw The yaw of the entity
 * @param pitch The pitch of the entity
 *
 * This is used to update the look of an entity
 * Value range is -128 to 127
 * Each unit is 1/256 of 360 degrees
 * For example, -128 would be -180 degrees
 */
typedef struct {
    int32_t entity_id;
    int8_t yaw, pitch;
} EP_EntityLookSC;

/**
 * Entity look and relative move packet from Server -> Client
 * @param entity_id The entity ID
 * @param dx The X delta
 * @param dy The Y delta
 * @param dz The Z delta
 * @param yaw The yaw of the entity
 * @param pitch The pitch of the entity
 *
 * This is used to update the look and move an entity relative to its current position
 * See EntityRelativeMoveSC and EntityLookSC for more information
 */
typedef struct {
    int32_t entity_id;
    int8_t dx, dy, dz;
    int8_t yaw, pitch;
} EP_EntityLookAndRelativeMoveSC;

/**
 * Entity teleport packet from Server -> Client
 * @param entity_id The entity ID
 * @param x The X position
 * @param y The Y position
 * @param z The Z position
 * @param yaw The yaw of the entity
 * @param pitch The pitch of the entity
 *
 * This is used to teleport an entity to a specific location
 */
typedef struct {
    int32_t entity_id;
    int32_t x, y, z;
    int8_t yaw, pitch;
} EP_EntityTeleportSC;

/**
 * Entity status packet from Server -> Client
 * @param entity_id The entity ID
 * @param status The status of the entity
 *
 * This is used to update the status of an entity
 */
typedef struct {
    int32_t entity_id;
    uint8_t status;
} EP_EntityStatusSC;

/**
 * Attach entity packet from Server -> Client
 * @param entity_id The entity ID
 * @param vehicle_id The vehicle ID
 *
 * This is used to attach an entity to a vehicle
 */
typedef struct {
    int32_t entity_id;
    int32_t vehicle_id;
} EP_AttachEntitySC;

/**
 * Level initialize packet from Server -> Client
 * @param seed The seed of the level
 *
 * This is used to initialize the level
 */
typedef struct {
    uint64_t seed;
} EP_LevelInitializeSC;

/**
 * Level data packet from Server -> Client
 * @param chunkLength The length of the chunk
 * @param chunkData The chunk data
 * @param percentComplete The percent complete
 *
 * //TODO: Include Lighting Data
 *
 * This is used to send the level data to the client
 */
typedef struct {
    int16_t chunkLength;
    uint8_t chunkData;
    uint8_t percentComplete;
} EP_LevelDataSC;

/**
 * Level finalize packet from Server -> Client
 * @param x The X size of the world
 * @param y The Y size of the world
 * @param z The Z size of the world
 *
 * This is used to finalize the level
 */
typedef struct {
    int16_t x, y, z;
} EP_LevelFinalizeSC;

/**
 * Multiple block change packet from Server -> Client
 * @param size The size of the packet
 * @param data The data of the packet -- TODO: Handle this
 */
typedef struct {
    uint16_t size;
    uint8_t* data;
} EP_MultiBlockChangeSC;

/**
 * Block change packet from Server -> Client
 * @param x The X position of the block
 * @param y The Y position of the block
 * @param z The Z position of the block
 * @param type The type of the block
 */
typedef struct {
    int32_t x;
    int8_t y;
    int32_t z;
    uint8_t type;
} EP_BlockChangeSC;

/**
 * Sends a tile entity packet from Server -> Client
 * @param x The X position of the tile entity
 * @param y The Y position of the tile entity
 * @param z The Z position of the tile entity
 * @param size The size of the tile entity
 * @param data The data of the tile entity -- TODO: Handle this It's GZIP'd NBT data
 *
 * This is used to send a tile entity to the client
 */
typedef struct {
    int32_t x;
    int8_t y;
    int32_t z;

    uint16_t size;
    uint8_t* data;
} EP_TileEntitySC;

/**
 * Cause an explosion packet from Server -> Client
 * @param x The X position of the explosion
 * @param y The Y position of the explosion
 * @param z The Z position of the explosion
 * @param radius The radius of the explosion
 * @param count The count of the explosion blocks destroyed
 * @param data The data of the explosion blocks -- TODO: Handle this
 *
 * This is used to cause an explosion
 */
typedef struct {
    double x, y, z;
    float radius;

    int32_t count;
    uint8_t* data;
} EP_ExplosionSC;

/**
 * Kick the client packet from Server -> Client
 * @param reason The reason for the kick
 */
typedef struct {
    net_string reason;
} EP_KickSC;

#ifdef __cplusplus
};
#endif
