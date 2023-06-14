#pragma once
#include <CC/netproto.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PacketType type;
    // Union of all packets structures defined above
    union {
        KeepAlivePacket keep_alive;
        LoginPacketCS login_cs;
        LoginPacketSC login_sc;
        HandshakePacketCS handshake_cs;
        HandShakePacketSC handshake_sc;
        ChatMessagePacket chat_message;
        TimeUpdatePacketSC time_update;
        EntityEquipmentPacketSC entity_equipment;
        SpawnPositionPacketSC spawn_position;
        UseEntityPacketCS use_entity;
        UpdateHealthPacketSC update_health;
        long respawn_data;
        PlayerCS player_cs;
        PlayerPositionCS player_position_cs;
        PlayerLookCS player_look_cs;
        PlayerPositionAndLookCS player_position_and_look_cs;
        PlayerPositionAndLookSC player_position_and_look_sc;
        PlayerDiggingCS player_digging_cs;
        PlayerPlaceCS player_place_cs;
        HeldItemChangeCS held_item_change_cs;
        AddToInventorySC add_to_inventory_sc;
        EntityAnimation entity_animation;
        SpawnNamedEntitySC spawn_named_entity_sc;
        SpawnDroppedItemSC spawn_dropped_item_sc;
        CollectItemSC collect_item_sc;
        SpawnObjectSC spawn_object_sc;
        SpawnMobSC spawn_mob_sc;
        EntityVelocitySC entity_velocity_sc;
        EntityDestroySC entity_destroy_sc;
        EntitySC entity_sc;
        EntityRelativeMoveSC entity_relative_move_sc;
        EntityLookSC entity_look_sc;
        EntityLookAndRelativeMoveSC entity_look_and_relative_move_sc;
        EntityTeleportSC entity_teleport_sc;
        EntityStatusSC entity_status_sc;
        AttachEntitySC attach_entity_sc;
        LevelInitializeSC level_initialize_sc;
        LevelDataSC level_data_sc;
        LevelFinalizeSC level_finalize_sc;
        MultiBlockChangeSC multi_block_change_sc;
        BlockChangeSC block_change_sc;
        TileEntitySC tile_entity_sc;
        ExplosionSC explosion_sc;
        KickSC kick_sc;
    } data;
} EventPacket;

typedef struct {
    EventPacket* packets;
    size_t length;
    size_t capacity;
} EventList;


typedef void (*EventHandler)(EventPacket* packet);

typedef struct {
    size_t count;
    EventHandler* handlers;
} EventMapping;

/**
 * Initializes the event loop.
 * This function must be called before any other event loop functions.
 */
void CC_EventLoop_Init(void);

/**
 * Shuts down the event loop.
 * This function must be called before the program exits.
 */
void CC_EventLoop_Shutdown(void);

/**
 * Registers a handler for a packet type.
 * @param type One of the PacketType enum values.
 * @param handler The handler function.
 */
void CC_EventLoop_RegisterHandler(PacketType type, EventHandler handler);

/**
 * Pushes a packet into the event loop.
 * @param packet The packet to push.
 */
void CC_EventLoop_PushPacketInbound(EventPacket* packet);

/**
 * Pushes a packet into the event loop.
 * @param packet The packet to push.
 */
void CC_EventLoop_PushPacketOutbound(EventPacket* packet);

/**
 * Deserializes a packet from a byte array.
 * @param data The byte array.
 * @param size The size of the byte array.
 * @return The deserialized packet.
 */
EventPacket* CC_EventLoop_DeserializePacket(uint8_t* data, size_t size);

/**
 * Frees a packet.
 * @param packet The packet to free.
 */
void CC_EventLoop_FreePacket(EventPacket* packet);

/**
 * Serializes a packet into a byte array.
 * @param packet Packet to serialize.
 * @param data Pointer to the byte array. This will be allocated by the function.
 * @return The size of the byte array.
 */
size_t CC_EventLoop_SerializePacket(EventPacket* packet, uint8_t** data);

/**
 * Sets whether the event loop is running on the server.
 * @param server Set to true if the event loop is running on the server.
 */
void CC_EventLoop_SetServer(bool server);

/**
 * Updates the event loop.
 * This function must be called every tick.
 * This function will call the handlers for all packets in the inbound queue.
 * This function will also send all packets in the outbound queue.
 */
void CC_EventLoop_Update(void);


#ifdef __cplusplus
}
#endif
