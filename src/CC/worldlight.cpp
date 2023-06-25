#include <CC/world.h>
#include <queue>
#include <stdio.h>

extern "C" {

struct LightNode {
	LightNode(size_t indx) : index(indx){}
	size_t index; //this is the x y z coordinate!
};

struct LightRemovalNode {
	LightRemovalNode(size_t indx, uint8_t val) : index(indx), value(val){}
	size_t index; //this is the x y z coordinate!
	uint8_t value;
};

std::queue<LightNode> lightBfsQueue;
std::queue<LightRemovalNode> lightRemovalBfsQueue;

std::queue<LightNode> sunLightBfsQueue;
std::queue<LightRemovalNode> sunLightRemovalBfsQueue;

void CC_World_SetLightLevel(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	auto sky = wd->lightmap[index] >> 4;
	wd->lightmap[index] = sky << 4 | (level & 0xF);
}

void CC_World_SetSunLightLevel(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	auto block = wd->lightmap[index] &0xF;
	wd->lightmap[index] = level << 4 | block;
}

void CC_World_LightSetCheckNeighbor(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	if(x >= wd->x || y >= wd->y || z >= wd->z) {
		return;
	}
	auto lightLevel = wd->lightmap[index] & 0xF;

	auto block = wd->blocks[index];

	auto boolMask = block == BLK_Air || block == BLK_Water || block == BLK_Glass || block == BLK_Leaves || block == BLK_Flower1 || block == BLK_Flower2 || block == BLK_Mushroom1 || block == BLK_Mushroom2;

	if (boolMask && lightLevel + 2 <= level) {
		CC_World_SetLightLevel(wd, x, y, z, level - 1);
		lightBfsQueue.emplace(index);
	}
}


void CC_World_LightSetCheckNeighborSunLight(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	if(x >= wd->x || y >= wd->y || z >= wd->z) {
		return;
	}
	auto lightLevel = wd->lightmap[index] >> 4;

	auto block = wd->blocks[index];

	auto boolMask = (block == BLK_Water || block == BLK_Glass || block == BLK_Leaves || block == BLK_Flower1 || block == BLK_Flower2 || block == BLK_Mushroom1 || block == BLK_Mushroom2);

	if ((block == BLK_Air || boolMask) && lightLevel + 2 <= level) {
		if(boolMask && level == 16) {
			if(level >= 2) {
				CC_World_SetSunLightLevel(wd, x, y, z,
							  level - 2);
			}  else {
				CC_World_SetSunLightLevel(wd, x, y, z, 0);
			}
		} else {
			CC_World_SetSunLightLevel(wd, x, y, z, level - 1);
		}
		sunLightBfsQueue.emplace(index);
	}
}

void CC_World_LightSetCheckNeighborRemove(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	if(x >= wd->x || y >= wd->y || z >= wd->z) {
		return;
	}
	auto neighborLevel = wd->lightmap[index] & 0xF;

	if(neighborLevel != 0 && neighborLevel < level) {
		CC_World_SetLightLevel(wd, x, y, z, 0);
		lightRemovalBfsQueue.emplace(index, neighborLevel);
	} else if (neighborLevel >= level) {
		lightBfsQueue.emplace(index);
	}
}

void CC_World_SunLightSetCheckNeighborRemove(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	if(x >= wd->x || y >= wd->y || z >= wd->z) {
		return;
	}
	auto neighborLevel = wd->lightmap[index] >> 4;

	if((neighborLevel != 0 && neighborLevel < level) || (level == 16)) {
		CC_World_SetSunLightLevel(wd, x, y, z, 0);
		sunLightRemovalBfsQueue.emplace(index, neighborLevel);
	} else if(neighborLevel >= level){
		sunLightBfsQueue.emplace(index);
	}
}

void CC_World_SetLight(size_t x, size_t y, size_t z, uint8_t light) {
	auto wd = CC_World_GetData();
	CC_World_SetLightLevel(wd, x, y, z, light);

	auto index = CC_WIDX(x, y, z, wd);
	lightBfsQueue.emplace(index);

	while(!lightBfsQueue.empty()) {
		LightNode &node = lightBfsQueue.front();
		auto idx = node.index;
		lightBfsQueue.pop();

		auto nX = idx % wd->x;
		auto nZ = (idx / wd->x) % wd->z;
		auto nY = idx / (wd->x * wd->z);

		auto lightLevel = wd->lightmap[idx] & 0xF;

		CC_World_LightSetCheckNeighbor(wd, nX + 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX - 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY + 1, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY - 1, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY, nZ + 1, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY, nZ - 1, lightLevel);
	}
}

void CC_World_SetSunLight(size_t x, size_t y, size_t z, uint8_t light) {
	auto wd = CC_World_GetData();
	CC_World_SetSunLightLevel(wd, x, y, z, light);

	auto index = CC_WIDX(x, y, z, wd);
	sunLightBfsQueue.emplace(index);

	while(!sunLightBfsQueue.empty()) {
		LightNode &node = sunLightBfsQueue.front();
		auto idx = node.index;
		sunLightBfsQueue.pop();

		auto nX = idx % wd->x;
		auto nZ = (idx / wd->x) % wd->z;
		auto nY = idx / (wd->x * wd->z);

		auto lightLevel = wd->lightmap[idx] >> 4;

		CC_World_LightSetCheckNeighborSunLight(wd, nX + 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighborSunLight(wd, nX - 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighborSunLight(wd, nX, nY, nZ + 1, lightLevel);
		CC_World_LightSetCheckNeighborSunLight(wd, nX, nY, nZ - 1, lightLevel);

		if(lightLevel == 15) {
			CC_World_LightSetCheckNeighborSunLight(wd, nX, nY - 1,
							       nZ, 16);
		} else {
			CC_World_LightSetCheckNeighborSunLight(wd, nX, nY - 1,
							       nZ, lightLevel);
		}
	}
}


void CC_World_SetSunLightFast(size_t x, size_t y, size_t z, uint8_t light) {
	auto wd = CC_World_GetData();
	CC_World_SetSunLightLevel(wd, x, y, z, light);
	for(int i = y; i >= 0; i--) {
		auto index = CC_WIDX(x, i, z, wd);
		auto block = wd->blocks[index];
		auto boolMask = (block == BLK_Water || block == BLK_Glass || block == BLK_Leaves || block == BLK_Flower1 || block == BLK_Flower2 || block == BLK_Mushroom1 || block == BLK_Mushroom2);

		if(boolMask) {
			light -= 2;
		}

		if(light > 0) {
			CC_World_SetSunLightLevel(wd, x, i, z, light);
		}
	}
	return;
}


void CC_World_RemoveSunLight(size_t x, size_t y, size_t z) {
	auto wd = CC_World_GetData();
	auto value = wd->lightmap[CC_WIDX(x, y, z, wd)] >> 4;
	CC_World_SetSunLightLevel(wd, x, y, z, 0);

	auto index = CC_WIDX(x, y, z, wd);
	sunLightRemovalBfsQueue.emplace(index, value);

	while(!sunLightRemovalBfsQueue.empty()) {
		LightRemovalNode &node = sunLightRemovalBfsQueue.front();
		auto idx = node.index;
		auto lightLevel = node.value;
		sunLightRemovalBfsQueue.pop();

		auto nX = idx % wd->x;
		auto nZ = (idx / wd->x) % wd->z;
		auto nY = idx / (wd->x * wd->z);

		CC_World_SunLightSetCheckNeighborRemove(wd, nX + 1, nY, nZ, lightLevel);
		CC_World_SunLightSetCheckNeighborRemove(wd, nX - 1, nY, nZ, lightLevel);
		CC_World_SunLightSetCheckNeighborRemove(wd, nX, nY, nZ + 1, lightLevel);
		CC_World_SunLightSetCheckNeighborRemove(wd, nX, nY, nZ - 1, lightLevel);

		auto lv = wd->lightmap[CC_WIDX(nX, nY, nZ, wd)] >> 4;
		if(lv == 0) {
			printf("removing sun light at %zu %zu %zu\n", nX, nY, nZ	);
			CC_World_SunLightSetCheckNeighborRemove(wd, nX, nY - 1,
								       nZ, 16);
		}
	}

	while(!sunLightBfsQueue.empty()) {
		LightNode &node = sunLightBfsQueue.front();
		auto idx = node.index;
		sunLightBfsQueue.pop();

		auto nX = idx % wd->x;
		auto nZ = (idx / wd->x) % wd->z;
		auto nY = idx / (wd->x * wd->z);

		auto lightLevel = wd->lightmap[idx] >> 4;

		CC_World_LightSetCheckNeighborSunLight(wd, nX + 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighborSunLight(wd, nX - 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighborSunLight(wd, nX, nY, nZ + 1, lightLevel);
		CC_World_LightSetCheckNeighborSunLight(wd, nX, nY, nZ - 1, lightLevel);
		CC_World_LightSetCheckNeighborSunLight(wd, nX, nY + 1, nZ, lightLevel);

		if(lightLevel == 15) {
			CC_World_LightSetCheckNeighborSunLight(wd, nX, nY - 1,
							       nZ, 16);
		} else {
			CC_World_LightSetCheckNeighborSunLight(wd, nX, nY - 1,
							       nZ, lightLevel);
		}
	}
}

void CC_World_RemoveLight(size_t x, size_t y, size_t z) {
	auto wd = CC_World_GetData();
	auto value = wd->lightmap[CC_WIDX(x, y, z, wd)] & 0xF;
	CC_World_SetLightLevel(wd, x, y, z, 0);

	auto index = CC_WIDX(x, y, z, wd);
	lightRemovalBfsQueue.emplace(index, value);

	while(!lightRemovalBfsQueue.empty()) {
		LightRemovalNode &node = lightRemovalBfsQueue.front();
		auto idx = node.index;
		auto lightLevel = node.value;
		lightRemovalBfsQueue.pop();

		auto nX = idx % wd->x;
		auto nZ = (idx / wd->x) % wd->z;
		auto nY = idx / (wd->x * wd->z);

		CC_World_LightSetCheckNeighborRemove(wd, nX + 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighborRemove(wd, nX - 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighborRemove(wd, nX, nY + 1, nZ, lightLevel);
		CC_World_LightSetCheckNeighborRemove(wd, nX, nY - 1, nZ, lightLevel);
		CC_World_LightSetCheckNeighborRemove(wd, nX, nY, nZ + 1, lightLevel);
		CC_World_LightSetCheckNeighborRemove(wd, nX, nY, nZ - 1, lightLevel);
	}

	while(!lightBfsQueue.empty()) {
		LightNode &node = lightBfsQueue.front();
		auto idx = node.index;
		lightBfsQueue.pop();

		auto nX = idx % wd->x;
		auto nZ = (idx / wd->x) % wd->z;
		auto nY = idx / (wd->x * wd->z);

		auto lightLevel = wd->lightmap[idx] & 0xF;

		CC_World_LightSetCheckNeighbor(wd, nX + 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX - 1, nY, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY + 1, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY - 1, nZ, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY, nZ + 1, lightLevel);
		CC_World_LightSetCheckNeighbor(wd, nX, nY, nZ - 1, lightLevel);
	}
}

}