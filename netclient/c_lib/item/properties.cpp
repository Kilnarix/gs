#include "properties.hpp"

#include <item/_interface.hpp>
#include <item/common/constant.hpp>
#include <item/common/struct.hpp>

#include <item/config/item_attribute.hpp>
#include <item/config/crafting_dat.hpp>
#include <item/item_container.hpp>

namespace Item
{


int sprite_array[MAX_ITEMS]; //maps item id to sprite
int group_array[MAX_ITEMS];
class ItemAttribute* item_attribute_array = NULL;
class NaniteStoreItem* nanite_store_item_array = NULL;
class CraftingRecipe* crafting_recipe_array = NULL;

// buffers for condensing craft bench inputs to unique type,count pairs
int craft_input_types[CRAFT_BENCH_INPUTS_MAX];
int craft_input_totals[CRAFT_BENCH_INPUTS_MAX];

// buffers for recipe outputs available
class CraftingRecipe* craft_recipes_possible[CRAFT_BENCH_OUTPUTS_MAX];
int craft_recipes_possible_count = 0;

void init_properties()
{
    for (int i=0; i<MAX_ITEMS; sprite_array[i++] = ERROR_SPRITE);
    for (int i=0; i<MAX_ITEMS; group_array[i++] = IG_ERROR);

    assert(item_attribute_array == NULL);
    item_attribute_array = new ItemAttribute[MAX_ITEMS];
    assert(nanite_store_item_array == NULL);
    nanite_store_item_array = new NaniteStoreItem[MAX_ITEMS];

    crafting_recipe_array = new CraftingRecipe[MAX_CRAFTING_RECIPE];
}

void tear_down_properties()
{
    if (item_attribute_array    != NULL) delete[] item_attribute_array;
    if (nanite_store_item_array != NULL) delete[] nanite_store_item_array;
    if (nanite_store_item_array != NULL) delete[] crafting_recipe_array;
}

class ItemAttribute* get_item_attributes(int item_type)
{
    return &item_attribute_array[item_type];
}

int get_item_fire_rate(int item_type)
{
    // TODO
    return 5;
}

int get_sprite_index_for_id(ItemID id)
{
    assert(id < MAX_ITEMS && id >= 0);
    int type = get_item_type(id);
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    assert(type >= 0 && type < MAX_ITEMS);
    return sprite_array[type];
}

int get_sprite_index_for_type(int type)
{
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    assert(type >= 0 && type < MAX_ITEMS);
    return sprite_array[type];
}

/*
Names
*/


const int ITEM_NAME_MAX_LENGTH = 64;
char item_names[MAX_ITEMS*ITEM_NAME_MAX_LENGTH];
int item_name_index[MAX_ITEMS];

void set_item_name(int id, char* name, int length)
{
    assert(length > 0);
    assert(id >= 0 || id < MAX_ITEMS);
    
    if (length >= ITEM_NAME_MAX_LENGTH)
    {
        printf("Error: %s, name length greater than 63 characters \n", __func__ );
        assert(length < ITEM_NAME_MAX_LENGTH);
    }

    static int index = 0;

    item_name_index[id] = index;

    memcpy(item_names+index, name, length);
    index += length;
    item_names[index] = '\0';
    index++;
}

void set_item_name(int id, char* name)
{
    int length = strlen(name);
    set_item_name(id, name, length);
}


char* get_item_name(int type)
{
    assert(type >= 0 || type < MAX_ITEMS);
    return (item_names + item_name_index[type]);
}

int get_item_type(char* name)
{
    for (int i=0; i<MAX_ITEMS; i++)
        if (strcmp(name, get_item_name(i)) == 0)
            return i;
    return NULL_ITEM_TYPE;
}

int get_item_group_for_type(int item_type)
{
    return group_array[item_type];
}

int dat_get_item_type(const char* name)
{
    int type = get_item_type((char*) name);
    if (type == NULL_ITEM_TYPE)
    {
        printf("Dat Loading Failure:item_type, dat failure, item '%s' does not exist! \n", name);
        GS_ABORT();
    }
    return type;
}

int get_max_stack_size(int item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    assert(attr != NULL);
    return attr->max_stack_size;
}

int get_max_energy(int item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    assert(attr != NULL);
    return attr->max_energy;
}

int get_max_durability(int item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    assert(attr != NULL);
    return attr->max_durability;
}

void get_nanite_store_item(int level, int xslot, int yslot, int* item_type, int* cost)
{
    for(int i=0; i<MAX_ITEMS; i++)
    {
        class NaniteStoreItem* n = &nanite_store_item_array[i];
        if(n->level == level && n->xslot == xslot && n->yslot == yslot)
        {
            *item_type = n->item_type;
            *cost = n->nanite_cost;
            return;
        }
    }
    *item_type = NULL_ITEM_TYPE;
    *cost = 0;
}

class CraftingRecipe* get_craft_recipe(int recipe_id)
{
    assert(recipe_id >= 0 && recipe_id < crafting_recipe_count);
    return &crafting_recipe_array[recipe_id];
}

class CraftingRecipe* get_selected_craft_recipe(int container_id, int slot)
{
    // get container
    assert(container_id != NULL_CONTAINER);
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return NULL;

    // clear input buffers
    for (int i=0; i<CRAFT_BENCH_INPUTS_MAX; craft_input_types[i++] = NULL_ITEM_TYPE);
    for (int i=0; i<CRAFT_BENCH_INPUTS_MAX; craft_input_totals[i++] = 0);

    // condense container contents to unique types/counts
    int unique_inputs = 0;
    for (int i=0; i<container->slot_max; i++)
    {
        // get slot content data
        ItemID item_id = container->get_item(i);
        if (item_id == NULL_ITEM) continue;
        int item_type = get_item_type(item_id);
        assert(item_type != NULL_ITEM_TYPE);    // item type should exist here, because we are skipping empty slots
        int stack_size = get_stack_size(item_id);
        assert(stack_size >= 1);

        // insert into type buffer
        if (unique_inputs == 0)
        {   // degenerate case
            craft_input_types[unique_inputs] = item_type;
            craft_input_totals[unique_inputs] = stack_size;
        }
        else
        {   // keep buffer sorted
            int i=0;
            for (; i<unique_inputs; i++)
            {
                if (craft_input_types[i] < item_type) continue;

                // shift forward
                for (int j=unique_inputs; j>i; j--) craft_input_types[j] = craft_input_types[j-1];
                for (int j=unique_inputs; j>i; j--) craft_input_totals[j] = craft_input_totals[j-1];

                // insert
                craft_input_types[i] = item_type;
                craft_input_totals[i] = stack_size;
                break;
            }
            if (i == unique_inputs)
            {   // append to end
                craft_input_types[unique_inputs] = item_type;
                craft_input_totals[unique_inputs] = stack_size;
            }
        }
        unique_inputs++;
    }

    // no inputs
    if (unique_inputs == 0) return NULL;

    // reset outputs buffer
    for (int i=0; i<CRAFT_BENCH_OUTPUTS_MAX; craft_recipes_possible[i++] = NULL);
    craft_recipes_possible_count = 0;
    
    // iterate available recipes
    // if types match exactly, add recipe to available recipes

    for (int i=0; i<crafting_recipe_count; i++)
    {
        CraftingRecipe* recipe = &crafting_recipe_array[i];
        assert(recipe->output != NULL_ITEM_TYPE);
        assert(recipe->reagent_num > 0);
        // make sure to set availability state to default
        recipe->available = true;

        // only match exactly
        if (recipe->reagent_num != unique_inputs) continue;

        // check if reagents match inputs
        bool match = true;
        bool can_craft = true;
        for (int j=0; j<recipe->reagent_num; j++)
        {
            if (recipe->reagent[j] == craft_input_types[j])
            {   // check for reagent counts
                if (recipe->reagent_count[j] > craft_input_totals[j]) can_craft = false;
            }
            else
            {
                match = false;
                break;
            }
        }
        if (!match) continue;

        if (!can_craft) recipe->available = false;
        craft_recipes_possible[craft_recipes_possible_count] = recipe;
        craft_recipes_possible_count++;
    }

    // slot is out of recipe range
    if (craft_recipes_possible_count < slot) return NULL;
    
    return craft_recipes_possible[slot];
}

int get_selected_craft_recipe_type(int container_id, int slot)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return NULL_ITEM_TYPE;
    if (!recipe->available) return dat_get_item_type((char*)"unknown");
    return recipe->output;
}

}
