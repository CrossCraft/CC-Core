#include <CC/type.h>

#if __cplusplus
extern "C" {
#endif

typedef enum {
    Item = 1,
} CC_EntityType;

typedef struct {
    int eid;
    uint8_t type;
    float x, y, z;
    float vx, vy, vz;
    uint8_t pitch, yaw;
    void* data;
} CC_Entity;

void CC_Entity_Init(void);
void CC_Entity_Term(void);

int16_t CC_Entity_Spawn(CC_Entity* entity);
void CC_Entity_Destroy(uint16_t eid);

void CC_Entity_Update(double dt);

int16_t CC_Entity_SpawnItem(float x, float y, float z, float vx, float vy, float vz, item_t item, item_t data, uint8_t count);

#if __cplusplus
}
#endif
