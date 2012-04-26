#pragma once

#include <c_lib/entity/constants.hpp>

typedef struct Stack
{
    int count;
    int max;
} Stack;

const int EMPTY_SLOT = 65535;
class InventorySlot
{
    public:
        int item_id;
        ObjectType item_type;
        int slot;
        Stack stack;

        bool empty()
        {
            if (this->item_id == EMPTY_SLOT) return true;
            return false;
        }

        void load(int id, ObjectType type, int stack_size);

        #if DC_CLIENT
        // render data
        int sprite_index;
        #endif

        void print()
        {
            printf("Slot %d,%d,%d\n", this->item_id, this->item_type, this->slot);
        }

    InventorySlot()
    :
    item_id(EMPTY_SLOT), item_type(OBJECT_NONE),
    slot(-1)    // slot is set after allocation
    #if DC_CLIENT
    , sprite_index(0)
    #endif
    {
        this->stack.max = 1;
        this->stack.count = 0;
    }
};
