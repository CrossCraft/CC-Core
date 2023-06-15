#pragma once
#include <CC/netproto.h>
#include <CC/databus.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    EP_PacketType type;
    // Union of all packets structures defined above
    union {
        EP_KeepAlivePacket keep_alive;
        EP_LoginPacketCS login_cs;
        EP_LoginPacketSC login_sc;
        EP_HandshakePacketCS handshake_cs;
        EP_HandShakePacketSC handshake_sc;
        EP_ChatMessagePacket chat_message;
        EP_TimeUpdatePacketSC time_update;
        EP_EntityEquipmentPacketSC entity_equipment;
        EP_SpawnPositionPacketSC spawn_position;
        EP_UseEntityPacketCS use_entity;
        EP_UpdateHealthPacketSC update_health;
        EP_RespawnDataSC* respawn_data;
        EP_PlayerCS player_cs;
        EP_PlayerPositionCS player_position_cs;
        EP_PlayerLookCS player_look_cs;
        EP_PlayerPositionAndLookCS player_position_and_look_cs;
        EP_PlayerPositionAndLookSC player_position_and_look_sc;
        EP_PlayerDiggingCS player_digging_cs;
        EP_PlayerPlaceCS player_place_cs;
        EP_HeldItemChangeCS held_item_change_cs;
        EP_AddToInventorySC add_to_inventory_sc;
        EP_EntityAnimation entity_animation;
        EP_SpawnNamedEntitySC spawn_named_entity_sc;
        EP_SpawnDroppedItemSC spawn_dropped_item_sc;
        EP_CollectItemSC collect_item_sc;
        EP_SpawnObjectSC spawn_object_sc;
        EP_SpawnMobSC spawn_mob_sc;
        EP_EntityVelocitySC entity_velocity_sc;
        EP_EntityDestroySC entity_destroy_sc;
        EP_EntitySC entity_sc;
        EP_EntityRelativeMoveSC entity_relative_move_sc;
        EP_EntityLookSC entity_look_sc;
        EP_EntityLookAndRelativeMoveSC entity_look_and_relative_move_sc;
        EP_EntityTeleportSC entity_teleport_sc;
        EP_EntityStatusSC entity_status_sc;
        EP_AttachEntitySC attach_entity_sc;
        EP_LevelInitializeSC level_initialize_sc;
        EP_LevelDataSC level_data_sc;
        EP_LevelFinalizeSC level_finalize_sc;
        EP_MultiBlockChangeSC multi_block_change_sc;
        EP_BlockChangeSC block_change_sc;
        EP_TileEntitySC tile_entity_sc;
        EP_ExplosionSC explosion_sc;
        EP_KickSC kick_sc;
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
void CC_EventLoop_RegisterHandler(EP_PacketType type, EventHandler handler);

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

/**
 * Sets the data bus for the event loop.
 * @param bus The data bus.
 */
void CC_EventLoop_SetBus(DataBus* bus);

#ifdef __cplusplus
}
#endif
