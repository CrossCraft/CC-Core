#include <CC/item.h>

ItemData CC_Item_Lookup_Block_Drop(block_t block) {
    ItemData item = {0};
    item.item_id = 0;
    item.damage = 0;
    item.count = 0;

    if(block == BLK_Grass) {
        item.item_id = BLK_Dirt;
        item.count = 1;
    } else if(block == BLK_Stone || block == BLK_Leaves) {
        return item;
    } else {
        item.item_id = block;
        item.count = 1;
    }

    return item;
}
