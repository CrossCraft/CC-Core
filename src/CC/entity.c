#include <CC/entity.h>
#include <stdlib.h>

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

int16_t CC_Entity_SpawnItem(float x, float y, float z, item_t item, item_t data, uint8_t count) {
    CC_Entity entity;
    entity.type = Item;
    entity.x = x + 0.5f;
    entity.y = y + 0.5f;
    entity.z = z + 0.5f;
    entity.pitch = 0;
    entity.yaw = 0;
    entity.data = malloc(sizeof(ItemData));
    ((ItemData*)entity.data)->id = item;
    ((ItemData*)entity.data)->data = data;
    ((ItemData*)entity.data)->count = count;
    return CC_Entity_Spawn(&entity);
}
