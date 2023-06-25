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

void CC_World_SetLightLevel(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	auto sky = wd->lightmap[index] >> 4;
	wd->lightmap[index] = sky << 4 | (level & 0xF);
}

void CC_World_LightSetCheckNeighbor(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	auto lightLevel = wd->lightmap[index] & 0xF;

	auto block = wd->blocks[index];

	auto boolMask = block == BLK_Air || block == BLK_Water || block == BLK_Glass || block == BLK_Leaves || block == BLK_Flower1 || block == BLK_Flower2 || block == BLK_Mushroom1 || block == BLK_Mushroom2;

	if (boolMask && lightLevel + 2 <= level) {
		CC_World_SetLightLevel(wd, x, y, z, level - 1);
		lightBfsQueue.emplace(index);
	}
}

void CC_World_LightSetCheckNeighborRemove(const WorldData* wd, size_t x, size_t y, size_t z, uint8_t level) {
	auto index = CC_WIDX(x, y, z, wd);
	auto neighborLevel = wd->lightmap[index] & 0xF;

	if(neighborLevel != 0 && neighborLevel < level) {
		CC_World_SetLightLevel(wd, x, y, z, 0);
		lightRemovalBfsQueue.emplace(index, neighborLevel);
	} else if (neighborLevel >= level) {
		lightBfsQueue.emplace(index);
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