#include <CC/core.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define FNL_IMPL
#include <FNL.h>

static WorldData CC_GLOBAL_world_data;
static fnl_state FNL_GLOBAL_state;
static int32_t CC_GLOBAL_world_seed;

void CC_World_Init(size_t x, size_t y, size_t z) {
    CC_GLOBAL_world_data.x = x;
    CC_GLOBAL_world_data.y = y;
    CC_GLOBAL_world_data.z = z;
    CC_GLOBAL_world_data.blocks = (block_t*)malloc(x * y * z * sizeof(block_t));
    memset(CC_GLOBAL_world_data.blocks, 0, x * y * z * sizeof(block_t));

    srand(time(NULL));
    CC_GLOBAL_world_seed = rand();

    FNL_GLOBAL_state = fnlCreateState();
    FNL_GLOBAL_state.seed = CC_GLOBAL_world_seed;
    FNL_GLOBAL_state.noise_type = FNL_NOISE_PERLIN;
    FNL_GLOBAL_state.frequency = 0.02f;
}

void CC_World_Term(void) {
    free(CC_GLOBAL_world_data.blocks);
}

const float WORLD_HEIGHT = 64.0f;


void setBlock(int x, int y, int z, block_t blockType) {
    CC_World_SetBlock(x, y, z, blockType);
}

void growTree(int x, int y, int z, int height) {
    int maxY = y + height;
    int maxLeafY = maxY + 1;
    int minLeaf = maxLeafY - 4;

    // Set the leaves
    for (int dx = -2; dx <= 2; dx++) {
        for (int dz = -2; dz <= 2; dz++) {
            for(int dy = 0; dy < height; dy++) {
                if(dy < 2) {
                    setBlock(x + dx, minLeaf + dy, z + dz, BLK_Leaves);

                    if((dx == 2 && dz == 2) || (dx == -2 && dz == 2) || (dx == 2 && dz == -2) || (dx == -2 && dz == -2)) {
                        if(rand() % 2) {
                            setBlock(x + dx, minLeaf + dy, z + dz, BLK_Air);
                        }
                    }
                }

                if(dy == 2) {
                    if((abs(dx) == 1 || abs(dz) == 1) && (abs(dx) + abs(dz)) ==  1) {
                        setBlock(x + dx, minLeaf + dy, z + dz, BLK_Leaves);
                    }

                    if((dx == 1 && dz == 1) || (dx == -1 && dz == 1) || (dx == 1 && dz == -1) || (dx == -1 && dz == -1)) {
                        if(rand() % 2) {
                            setBlock(x + dx, minLeaf + dy, z + dz, BLK_Leaves);
                        }
                    }
                }

                if(dy == 3) {
                    if((abs(dx) == 1 || abs(dz) == 1) && (abs(dx) + abs(dz)) ==  1) {
                        setBlock(x + dx, minLeaf + dy, z + dz, BLK_Leaves);
                    }
                }
            }
        }
    }

    // Set the trunk
    for (int i = 0; i < height; i++) {
        setBlock(x, y + i, z, BLK_Logs);
    }
}

void CC_World_Make_Tree(size_t ix, size_t iy, size_t iz) {
    int tree_height = 4 + rand() % 3;

    int lowx = ix - 2;
    int lowz = iz - 2;
    int highx = ix + 2;
    int highz = iz + 2;

    // Check if there is enough space for the tree
    for(int x = lowx; x <= highx; x++) {
        for(int z = lowz; z <= highz; z++) {
            for(int y = iy; y < (int)(iy + tree_height); y++) {
                block_t block;
                CC_World_GetBlock(x, y, z, &block);
                if(block != BLK_Air) {
                    return;
                }
            }
        }
    }

    growTree(ix, iy, iz, tree_height);
}

void CC_World_Generate(void) {
    size_t x, y, z;
    size_t idx;
    size_t h;

    float* noiseMap = (float*)malloc(CC_GLOBAL_world_data.x * CC_GLOBAL_world_data.z * sizeof(float));

    for(z = 0; z < CC_GLOBAL_world_data.z; z++) {
        for(x = 0; x < CC_GLOBAL_world_data.x; x++) {
            float normalized = (fnlGetNoise2D(&FNL_GLOBAL_state, x, z) * 0.5f + 0.5f); //Bounded 0.0f to 1.0f
            noiseMap[z * CC_GLOBAL_world_data.x + x] = (normalized * 0.5f + 0.25f) * WORLD_HEIGHT; //Bounded 0.25f to 0.75f of WORLD_HEIGHT
            // AKA 16.0f to 48.0f
        }
    }

    for(x = 0; x < CC_GLOBAL_world_data.x; x++) {
        for(z = 0; z < CC_GLOBAL_world_data.z; z++) {
            h = (size_t)noiseMap[z * CC_GLOBAL_world_data.x + x];
            for(y = 0; y < h; y++) {
                idx = CC_WIDX(x, y, z, &CC_GLOBAL_world_data);

                if(y == 0) {
                    CC_GLOBAL_world_data.blocks[idx] = BLK_Bedrock;
                } else if(y == 1) {
                    CC_GLOBAL_world_data.blocks[idx] = BLK_Lava;
                } else if(y < (h - 4)) {
                    CC_GLOBAL_world_data.blocks[idx] = BLK_Stone;
                } else if(y >= (h - 4) && y < (h - 1)) {
                    CC_GLOBAL_world_data.blocks[idx] = BLK_Dirt;
                } else {
                    if(h < 32) {
                        float determinant = fnlGetNoise2D(&FNL_GLOBAL_state, (float)z, (float)x) - 0.0f;

                        if(determinant > 0.0f) {
                            CC_GLOBAL_world_data.blocks[idx] = BLK_Gravel;
                        } else {
                            CC_GLOBAL_world_data.blocks[idx] = BLK_Dirt;
                        }
                    } else if(h == 32) {
                        CC_GLOBAL_world_data.blocks[idx] = BLK_Sand;
                    } else {
                        CC_GLOBAL_world_data.blocks[idx] = BLK_Grass;
                    }
                }



            }

            if (h < 32) {
                for (y = h; y < 32; y++) {
                    idx = CC_WIDX(x, y, z, &CC_GLOBAL_world_data);
                    CC_GLOBAL_world_data.blocks[idx] = BLK_Water;
                }
            }

        }
    }

    // Post-generation
    srand(CC_GLOBAL_world_seed);
    for(x = 0; x < CC_GLOBAL_world_data.x; x++) {
        for(z = 0; z < CC_GLOBAL_world_data.z; z++) {
            h = (size_t)noiseMap[z * CC_GLOBAL_world_data.x + x];
            idx = CC_WIDX(x, h - 1, z, &CC_GLOBAL_world_data);

            block_t check = CC_GLOBAL_world_data.blocks[idx];
            idx = CC_WIDX(x, h, z, &CC_GLOBAL_world_data);

            if(check == BLK_Grass) {
                if((rand() % 100) < 10) {
                    CC_World_Make_Tree(x, h, z);
                }
            }
        }
    }

    free(noiseMap);
}

void CC_World_GetChunkSize(size_t* x, size_t* y, size_t* z) {
    *x = CC_GLOBAL_world_data.x / 16;
    *y = CC_GLOBAL_world_data.y / 16;
    *z = CC_GLOBAL_world_data.z / 16;
}

void CC_World_GetSize(size_t* x, size_t* y, size_t* z) {
    *x = CC_GLOBAL_world_data.x;
    *y = CC_GLOBAL_world_data.y;
    *z = CC_GLOBAL_world_data.z;
}

const WorldData* CC_World_GetData(void) {
    return &CC_GLOBAL_world_data;
}

void CC_World_GetBlock(size_t x, size_t y, size_t z, block_t* block) {
    *block = CC_GLOBAL_world_data.blocks[CC_WIDX(x, y, z, &CC_GLOBAL_world_data)];
}

void CC_World_SetBlock(size_t x, size_t y, size_t z, block_t block) {
    CC_GLOBAL_world_data.blocks[CC_WIDX(x, y, z, &CC_GLOBAL_world_data)] = block;
}

bool CC_World_TryGetBlock(size_t x, size_t y, size_t z, block_t* block) {
    if(x >= CC_GLOBAL_world_data.x || y >= CC_GLOBAL_world_data.y || z >= CC_GLOBAL_world_data.z) {
        return false;
    }

    *block = CC_GLOBAL_world_data.blocks[CC_WIDX(x, y, z, &CC_GLOBAL_world_data)];
    return true;
}
