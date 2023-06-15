#include <CC/entity.h>
#include <stdlib.h>
#include <CC/event.h>
#include <CC/world.h>

#define MAX_ENTITIES 1024

typedef struct {
    CC_Entity entities[MAX_ENTITIES];
    bool inUse[MAX_ENTITIES];
    int freeListHead;
} EntityManager;

EntityManager CC_GLOBAL_EntityManager;

void CC_Entity_Init(void) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        CC_GLOBAL_EntityManager.inUse[i] = false;
    }
    CC_GLOBAL_EntityManager.freeListHead = 0;
}

void CC_Entity_Term(void) {
    // Nothing to do here
}

int16_t CC_Entity_Spawn(CC_Entity* entity) {
    if(CC_GLOBAL_EntityManager.freeListHead >= MAX_ENTITIES) {
        CC_GLOBAL_EntityManager.freeListHead = 0;
    }

    int16_t max_iterations = MAX_ENTITIES;
    while(CC_GLOBAL_EntityManager.inUse[CC_GLOBAL_EntityManager.freeListHead] && max_iterations > 0) {
        CC_GLOBAL_EntityManager.freeListHead++;
        max_iterations--;
    }

    if(max_iterations == 0) {
        return -1;
    }

    int eid = CC_GLOBAL_EntityManager.freeListHead;
    CC_GLOBAL_EntityManager.freeListHead++;
    CC_GLOBAL_EntityManager.inUse[eid] = true;
    entity->eid = eid;
    CC_GLOBAL_EntityManager.entities[eid] = *entity;

    return eid;
}

void CC_Entity_Destroy(uint16_t eid) {
    CC_Entity* entity = &CC_GLOBAL_EntityManager.entities[eid];
    CC_GLOBAL_EntityManager.inUse[eid] = false;
    free(entity->data);
}

int16_t CC_Entity_SpawnItem(float x, float y, float z, float vx, float vy, float vz, item_t item, item_t data, uint8_t count) {
    CC_Entity entity;
    entity.type = ItemDrop;
    entity.x = x + 0.5f;
    entity.y = y + 0.5f;
    entity.z = z + 0.5f;
    entity.vx = vx;
    entity.vy = vy;
    entity.vz = vz;
    entity.pitch = 0;
    entity.yaw = 0;
    entity.data = malloc(sizeof(Item));
    ((Item*)entity.data)->item_id = item;
    ((Item*)entity.data)->damage = data;
    ((Item*)entity.data)->count = count;
    return CC_Entity_Spawn(&entity);
}

bool CheckCollideAxis(int x, int y, int z) {
    block_t block;
    if (CC_World_TryGetBlock(x, y, z, &block)) {
        return block != BLK_Air && block != BLK_Water;
    }

    return false;
}

void CC_Entity_CheckCollide(uint16_t eid, double dt) {
    CC_Entity* entity = &CC_GLOBAL_EntityManager.entities[eid];

    float x = entity->x + entity->vx * dt;
    float y = entity->y + entity->vy * dt;
    float z = entity->z + entity->vz * dt;

    int x0 = (int)(x);
    int y0 = (int)(y - 0.5f);
    int z0 = (int)(z);

    if(CheckCollideAxis(x0, entity->y, entity->z)) {
        entity->vx = 0;
    }

    if(CheckCollideAxis(entity->x, y0, entity->z)) {
        entity->vy = 0;
    }

    if(CheckCollideAxis(entity->x, entity->y, z0)) {
        entity->vz = 0;
    }

    if(CheckCollideAxis(entity->x, entity->y, entity->z)) {
        entity->vx = 0;
        entity->vz = 0;
    }
}

float entityTickTimer = 0.0f;
void CC_Entity_Update(double dt) {
    entityTickTimer += dt;

    if(entityTickTimer < 0.05f) {
        return;
    }
    entityTickTimer = 0.0f;

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (CC_GLOBAL_EntityManager.inUse[i]) {
            CC_Entity* entity = &CC_GLOBAL_EntityManager.entities[i];
            switch (entity->type) {
                case ItemDrop: {
                    // Check collisions
                    CC_Entity_CheckCollide(entity->eid, 0.05f);

                    entity->x += entity->vx * 0.05f;
                    entity->y += entity->vy * 0.05f;
                    entity->z += entity->vz * 0.05f;

                    entity->vy -= 16.0f * 0.05f;
                    entity->vx *= 0.85f;
                    entity->vz *= 0.85f;

                    CC_Event_Push_EntityTeleport(entity->eid, entity->x, entity->y, entity->z, entity->vx, entity->vy, entity->vz, entity->pitch, entity->yaw);
                    break;
                }
            }
        }
    }
}
