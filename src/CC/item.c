#include <CC/item.h>

ItemData CC_Item_Lookup_Block_Drop(block_t block) {
    ItemData item = {0};
    item.id = 0;
    item.data = 0;
    item.count = 0;

    if(block == BLK_Grass) {
        item.id = BLK_Dirt;
        item.count = 1;
    }

    return item;
}
