#include <CC/eventloop.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <CC/alphaindev.pb-c.h>
#include <stdio.h>
#include <CC/netproto.h>

EventLoop* CC_EventLoop_Init(void) {
    EventLoop* loop = malloc(sizeof(EventLoop));
    if(loop == NULL) {
        return NULL;
    }
    memset(loop, 0, sizeof(EventLoop));

    loop->inbound_events.packets = malloc(sizeof(EventPacket) * 16);
    loop->inbound_events.length = 0;
    loop->inbound_events.capacity = 16;

    loop->outbound_events.packets = malloc(sizeof(EventPacket) * 16);
    loop->outbound_events.length = 0;
    loop->outbound_events.capacity = 16;

    return loop;
}

void CC_EventLoop_Destroy(EventLoop* loop) {
    for(int i = 0; i < 256; i++) {
        if(loop->mapping[i].count > 0) {
            free(loop->mapping[i].handlers);
        }
    }
    free(loop->inbound_events.packets);
    free(loop->outbound_events.packets);
    free(loop);
}

void CC_EventLoop_RegisterHandler(EventLoop* loop, EP_PacketType type, EventHandler handler) {
    EventMapping* emap = &loop->mapping[type];
    if(emap->count == 0) {
        emap->handlers = malloc(sizeof(EventHandler));
    } else {
        emap->handlers = realloc(emap->handlers, sizeof(EventHandler) * (emap->count + 1));
    }
    emap->count++;
    emap->handlers[emap->count - 1] = handler;
}

void CC_EventLoop_PushPacketInbound(EventLoop* loop, EventPacket* packet) {
    if(loop->inbound_events.length == loop->inbound_events.capacity) {
        loop->inbound_events.capacity *= 2;
        loop->inbound_events.packets = realloc(loop->inbound_events.packets, sizeof(EventPacket) * loop->inbound_events.capacity);
    }
    loop->inbound_events.packets[loop->inbound_events.length] = *packet;
    loop->inbound_events.length++;
}

void CC_EventLoop_PushPacketOutbound(EventLoop* loop, EventPacket* packet) {
    if(loop->outbound_events.length == loop->outbound_events.capacity) {
        loop->outbound_events.capacity *= 2;
        loop->outbound_events.packets = realloc(loop->outbound_events.packets, sizeof(EventPacket) * loop->outbound_events.capacity);
    }
    loop->outbound_events.packets[loop->outbound_events.length] = *packet;
    loop->outbound_events.length++;
}

void CC_EventLoop_Update(EventLoop* loop) {
    // Serialize inbound packets
    size_t input_length;
    uint8_t* data = NULL;
    while((input_length = loop->inbound_bus->read(&data, loop->inbound_context)) != 0) {
        EventPacket packet = CC_EventLoop_DeserializePacket(loop, data, input_length);
        CC_EventLoop_PushPacketInbound(loop, &packet);
        free(data);
    }

    // Handle inbound packets
    for(size_t i = 0; i < loop->inbound_events.length; i++) {
        EventPacket packet = loop->inbound_events.packets[i];
        EventMapping* emap = &loop->mapping[packet.type];
        for(size_t j = 0; j < emap->count; j++) {
            emap->handlers[j](loop, &packet);
        }
        CC_EventLoop_FreePacket(packet, loop->server);
    }
    loop->inbound_events.length = 0;

    // Handle outbound packets
    for(size_t i = 0; i < loop->outbound_events.length; i++) {
        EventPacket packet = loop->outbound_events.packets[i];
        size_t output_length;
        uint8_t* out_data = NULL;
        output_length = CC_EventLoop_SerializePacket(loop, packet, &out_data);
        loop->outbound_bus->write(out_data, output_length, loop->outbound_context);
        free(out_data);
        CC_EventLoop_FreePacket(packet, !loop->server);
    }
    loop->outbound_events.length = 0;
}

void CC_EventLoop_SetInboundBus(EventLoop* loop, DataBus* bus, void* context) {
    loop->inbound_bus = bus;
    loop->inbound_context = context;
}

void CC_EventLoop_SetOutboundBus(EventLoop* loop, DataBus* bus, void* context) {
    loop->outbound_bus = bus;
    loop->outbound_context = context;
}

void CC_EventLoop_SetServer(EventLoop* loop, bool server) {
    loop->server = server;
}

void CC_EventLoop_FreePacket(EventPacket packet, bool cs) {
    switch(packet.type) {
        case CC_PACKET_TYPE_HANDSHAKE: {
            free(packet.data.handshake_cs.username.data);
            break;
        }
        case CC_PACKET_TYPE_LOGIN: {
            if(cs) {
                free(packet.data.login_cs.username.data);
                free(packet.data.login_cs.password.data);
            }
            break;
        }

	    case CC_PACKET_TYPE_TIME_UPDATE:
        case CC_PACKET_TYPE_SPAWN_POSITION:
        case CC_PACKET_TYPE_UPDATE_HEALTH:
        case CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK:
        case CC_PACKET_TYPE_PLAYER_DIGGING:
        case CC_PACKET_TYPE_BLOCK_CHANGE:
        case CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT: {
            break;
        }

        default: {
            printf("Unhandled Packet type: %d\n", packet.type);
            break;
        }
    }
}

EventPacket CC_EventLoop_DeserializePacket(EventLoop* loop, uint8_t* data, size_t size) {
    GeneralPacket* gp = general_packet__unpack(NULL, size, data);
    EventPacket packet;

    packet.type = (EP_PacketType) gp->packet_type;

    switch(packet.type) {
        case CC_PACKET_TYPE_HANDSHAKE: {
            if(loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_CS);
                packet.data.handshake_cs.username.length = gp->handshake_packet_cs->username->length;
                packet.data.handshake_cs.username.data = malloc(packet.data.handshake_cs.username.length);
                memcpy(packet.data.handshake_cs.username.data, gp->handshake_packet_cs->username->data.data, packet.data.handshake_cs.username.length);
            } else {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_SC);
                packet.data.handshake_sc.response.length = gp->handshake_packet_sc->response->length;
                packet.data.handshake_sc.response.data = malloc(packet.data.handshake_sc.response.length);
                memcpy(packet.data.handshake_sc.response.data, gp->handshake_packet_sc->response->data.data, packet.data.handshake_sc.response.length);
            }
            break;
        }
        case CC_PACKET_TYPE_LOGIN: {
            if(loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_LOGIN_PACKET_CS);
                packet.data.login_cs.username.length = gp->login_packet_cs->username->length;
                packet.data.login_cs.username.data = malloc(packet.data.login_cs.username.length);
                memcpy(packet.data.login_cs.username.data, gp->login_packet_cs->username->data.data, packet.data.login_cs.username.length);

                packet.data.login_cs.password.length = gp->login_packet_cs->password->length;
                packet.data.login_cs.password.data = malloc(packet.data.login_cs.password.length);
                memcpy(packet.data.login_cs.password.data, gp->login_packet_cs->password->data.data, packet.data.login_cs.password.length);

                packet.data.login_cs.protocol_version = gp->login_packet_cs->protocol_version;
                packet.data.login_cs.dimension = gp->login_packet_cs->dimension;
                packet.data.login_cs.seed = gp->login_packet_cs->seed;
            } else {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_LOGIN_PACKET_SC);
                memcpy(packet.data.login_sc.zeroes, gp->login_packet_sc->zeroes.data, 8);
            }
            break;
        }

        case CC_PACKET_TYPE_SPAWN_POSITION: {
            if(!loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_SPAWN_POSITION_PACKET_SC);
                packet.data.spawn_position.x = gp->spawn_position_packet_sc->x;
                packet.data.spawn_position.y = gp->spawn_position_packet_sc->y;
                packet.data.spawn_position.z = gp->spawn_position_packet_sc->z;
            }
            break;
        }

	    case CC_PACKET_TYPE_TIME_UPDATE: {
            if(!loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_TIME_UPDATE_PACKET_SC);
                packet.data.time_update.time = gp->time_update_packet_sc->time;
            }
            break;
	    }

        case CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT: {
            if(loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_PLAYER_BLOCK_PLACEMENT_PACKET_CS);
                packet.data.player_place_cs.x = gp->player_place_cs->x;
                packet.data.player_place_cs.y = gp->player_place_cs->y;
                packet.data.player_place_cs.z = gp->player_place_cs->z;
                packet.data.player_place_cs.face = gp->player_place_cs->face;
                packet.data.player_place_cs.item_id = gp->player_place_cs->item_id;
            }
            break;
        }

        case CC_PACKET_TYPE_PLAYER_DIGGING: {
            if(loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_PLAYER_DIGGING_PACKET_CS);
                packet.data.player_digging_cs.x = gp->player_digging_cs->x;
                packet.data.player_digging_cs.y = gp->player_digging_cs->y;
                packet.data.player_digging_cs.z = gp->player_digging_cs->z;
                packet.data.player_digging_cs.status = gp->player_digging_cs->status;
                packet.data.player_digging_cs.face = gp->player_digging_cs->face;
            }
            break;
        }

        case CC_PACKET_TYPE_UPDATE_HEALTH: {
            if(!loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_UPDATE_HEALTH_PACKET_SC);
                packet.data.update_health.health = gp->update_health_packet_sc->health;
            }
            break;
        }

        case CC_PACKET_TYPE_RESPAWN: {
            break;
        }

        case CC_PACKET_TYPE_BLOCK_CHANGE: {
            if(!loop->server) {
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_BLOCK_CHANGE_PACKET_SC);
                packet.data.block_change_sc.x = gp->block_change_sc->x;
                packet.data.block_change_sc.y = gp->block_change_sc->y;
                packet.data.block_change_sc.z = gp->block_change_sc->z;
                packet.data.block_change_sc.type = gp->block_change_sc->type;
            }
            break;
        }

            case CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK: {
            if(loop->server) {
                // This is CS
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_PLAYER_POSITION_AND_LOOK_CS);
                packet.data.player_position_and_look_cs.x = gp->player_position_and_look_cs->x;
                packet.data.player_position_and_look_cs.y = gp->player_position_and_look_cs->y;
                packet.data.player_position_and_look_cs.z = gp->player_position_and_look_cs->z;
                packet.data.player_position_and_look_cs.yaw = gp->player_position_and_look_cs->yaw;
                packet.data.player_position_and_look_cs.pitch = gp->player_position_and_look_cs->pitch;
                packet.data.player_position_and_look_cs.on_ground = gp->player_position_and_look_cs->on_ground;
            } else {
                // This is SC
                assert(gp->packet_content_case == GENERAL_PACKET__PACKET_CONTENT_PLAYER_POSITION_AND_LOOK_SC);
                packet.data.player_position_and_look_sc.x = gp->player_position_and_look_sc->x;
                packet.data.player_position_and_look_sc.y = gp->player_position_and_look_sc->y;
                packet.data.player_position_and_look_sc.z = gp->player_position_and_look_sc->z;
                packet.data.player_position_and_look_sc.yaw = gp->player_position_and_look_sc->yaw;
                packet.data.player_position_and_look_sc.pitch = gp->player_position_and_look_sc->pitch;
                packet.data.player_position_and_look_sc.on_ground = gp->player_position_and_look_sc->on_ground;
            }
            break;
        }

        default: {
            printf("Unhandled Packet type: %d\n", gp->packet_type);
            break;
        }
    }

    general_packet__free_unpacked(gp, NULL);
    return packet;
}

size_t CC_EventLoop_SerializePacket(EventLoop* loop, EventPacket packet, uint8_t** data) {
    GeneralPacket gp = GENERAL_PACKET__INIT;
    size_t size = 0;

    switch(packet.type) {
        case CC_PACKET_TYPE_RESPAWN: {
            gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_RESPAWN;
            gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_RESPAWN_DATA_SC;
            RespawnDataSC rsp = RESPAWN_DATA_SC__INIT;
            gp.respawn_data_sc = &rsp;
            size = general_packet__get_packed_size(&gp);
            *data = malloc(size);
            general_packet__pack(&gp, *data);
            break;
        }

        case CC_PACKET_TYPE_HANDSHAKE: {
            if(loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_HANDSHAKE;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_SC;
                HandshakePacketSC hsc = HANDSHAKE_PACKET_SC__INIT;

                NetString response = NET_STRING__INIT;
                response.length = packet.data.handshake_sc.response.length;
                response.data.len = packet.data.handshake_sc.response.length;
                response.data.data = malloc(packet.data.handshake_sc.response.length);
                memcpy(response.data.data, packet.data.handshake_sc.response.data, packet.data.handshake_sc.response.length);
                hsc.response = &response;

                gp.handshake_packet_sc = &hsc;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
                free(hsc.response->data.data);
            } else {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_HANDSHAKE;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_HANDSHAKE_PACKET_CS;
                HandshakePacketCS hcs = HANDSHAKE_PACKET_CS__INIT;

                NetString response = NET_STRING__INIT;
                response.length = packet.data.handshake_sc.response.length;
                response.data.len = packet.data.handshake_sc.response.length;
                response.data.data = malloc(packet.data.handshake_sc.response.length);
                memcpy(response.data.data, packet.data.handshake_sc.response.data, packet.data.handshake_sc.response.length);
                hcs.username = &response;

                gp.handshake_packet_cs = &hcs;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
                free(hcs.username->data.data);
            }
            break;
        }

        case CC_PACKET_TYPE_BLOCK_CHANGE: {
            if(loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_BLOCK_CHANGE;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_BLOCK_CHANGE_SC;
                BlockChangeSC bsc = BLOCK_CHANGE_SC__INIT;
                bsc.x = packet.data.block_change_sc.x;
                bsc.y = packet.data.block_change_sc.y;
                bsc.z = packet.data.block_change_sc.z;
                bsc.type = packet.data.block_change_sc.type;
                gp.block_change_sc = &bsc;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            }
            break;
        }

        case CC_PACKET_TYPE_LOGIN: {
            if(loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_LOGIN;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_LOGIN_PACKET_SC;
                LoginPacketSC lsc = LOGIN_PACKET_SC__INIT;
                lsc.zeroes.len = 8;
                lsc.zeroes.data = malloc(8);
                memcpy(lsc.zeroes.data, packet.data.login_sc.zeroes, 8);
                gp.login_packet_sc = &lsc;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
                free(lsc.zeroes.data);
            } else {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_LOGIN;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_LOGIN_PACKET_CS;
                LoginPacketCS lcs = LOGIN_PACKET_CS__INIT;

                NetString username = NET_STRING__INIT;
                username.length = packet.data.login_cs.username.length;
                username.data.len = packet.data.login_cs.username.length;
                username.data.data = malloc(packet.data.login_cs.username.length);
                memcpy(username.data.data, packet.data.login_cs.username.data, packet.data.login_cs.username.length);
                lcs.username = &username;

                NetString password = NET_STRING__INIT;
                password.length = packet.data.login_cs.password.length;
                password.data.len = packet.data.login_cs.password.length;
                password.data.data = malloc(packet.data.login_cs.password.length);
                memcpy(password.data.data, packet.data.login_cs.password.data, packet.data.login_cs.password.length);
                lcs.password = &password;

                lcs.protocol_version = packet.data.login_cs.protocol_version;
                lcs.dimension = packet.data.login_cs.dimension;
                lcs.seed = packet.data.login_cs.seed;

                gp.login_packet_cs = &lcs;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
                free(lcs.username->data.data);
                free(lcs.password->data.data);
            }
            break;
        }

        case CC_PACKET_TYPE_SPAWN_POSITION: {
            if(loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_SPAWN_POSITION;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_SPAWN_POSITION_PACKET_SC;
                SpawnPositionPacketSC spsc = SPAWN_POSITION_PACKET_SC__INIT;
                spsc.x = packet.data.spawn_position.x;
                spsc.y = packet.data.spawn_position.y;
                spsc.z = packet.data.spawn_position.z;

                gp.spawn_position_packet_sc = &spsc;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            }
            break;
        }

        case CC_PACKET_TYPE_PLAYER_DIGGING: {
            if(!loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_PLAYER_DIGGING;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_PLAYER_DIGGING_CS;
                PlayerDiggingCS pdcs = PLAYER_DIGGING_CS__INIT;
                pdcs.status = packet.data.player_digging_cs.status;
                pdcs.x = packet.data.player_digging_cs.x;
                pdcs.y = packet.data.player_digging_cs.y;
                pdcs.z = packet.data.player_digging_cs.z;
                pdcs.face = packet.data.player_digging_cs.face;

                gp.player_digging_cs = &pdcs;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            }
            break;
        }

        case CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT: {
            if(!loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_PLAYER_BLOCK_PLACEMENT;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_PLAYER_PLACE_CS;
                PlayerPlaceCS ppcs = PLAYER_PLACE_CS__INIT;
                ppcs.x = packet.data.player_place_cs.x;
                ppcs.y = packet.data.player_place_cs.y;
                ppcs.z = packet.data.player_place_cs.z;
                ppcs.face = packet.data.player_place_cs.face;
                ppcs.item_id = packet.data.player_place_cs.item_id;

                gp.player_place_cs = &ppcs;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            }
            break;
        }

        case CC_PACKET_TYPE_UPDATE_HEALTH: {
            if(loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_UPDATE_HEALTH;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_UPDATE_HEALTH_PACKET_SC;
                UpdateHealthPacketSC uhp = UPDATE_HEALTH_PACKET_SC__INIT;
                uhp.health = packet.data.update_health.health;

                gp.update_health_packet_sc = &uhp;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            }
            break;
        }

	    case CC_PACKET_TYPE_TIME_UPDATE: {
	        if(loop->server) {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_TIME_UPDATE;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_TIME_UPDATE_PACKET_SC;
                TimeUpdatePacketSC tupsc = TIME_UPDATE_PACKET_SC__INIT;
                tupsc.time = packet.data.time_update.time;

                gp.time_update_packet_sc = &tupsc;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            }
	        break;
	    }

        case CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK: {
            if(!loop->server) {
                //Client packet
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_PLAYER_POSITION_AND_LOOK_CS;
                PlayerPositionAndLookCS ppls = PLAYER_POSITION_AND_LOOK_CS__INIT;
                ppls.x = packet.data.player_position_and_look_cs.x;
                ppls.y = packet.data.player_position_and_look_cs.y;
                ppls.z = packet.data.player_position_and_look_cs.z;
                ppls.yaw = packet.data.player_position_and_look_cs.yaw;
                ppls.pitch = packet.data.player_position_and_look_cs.pitch;
                ppls.on_ground = packet.data.player_position_and_look_cs.on_ground;

                gp.player_position_and_look_cs = &ppls;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            } else {
                gp.packet_type = PACKET_TYPE__CC_PACKET_TYPE_PLAYER_POSITION_AND_LOOK;
                gp.packet_content_case = GENERAL_PACKET__PACKET_CONTENT_PLAYER_POSITION_AND_LOOK_SC;
                PlayerPositionAndLookSC ppls = PLAYER_POSITION_AND_LOOK_SC__INIT;
                ppls.x = packet.data.player_position_and_look_sc.x;
                ppls.y = packet.data.player_position_and_look_sc.y;
                ppls.z = packet.data.player_position_and_look_sc.z;
                ppls.yaw = packet.data.player_position_and_look_sc.yaw;
                ppls.pitch = packet.data.player_position_and_look_sc.pitch;
                ppls.on_ground = packet.data.player_position_and_look_sc.on_ground;

                gp.player_position_and_look_sc = &ppls;
                size = general_packet__get_packed_size(&gp);
                *data = malloc(size);
                general_packet__pack(&gp, *data);
            }
            break;
        }

        default: {
            printf("Unhandled Packet type: %d\n", packet.type);
            break;
        }
    }

    return size;
}
