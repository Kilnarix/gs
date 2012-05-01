#pragma once

//typedef int ItemID;

#include <item/common/enum.hpp>

namespace Item
{

void init();
void teardown();
class ItemContainer* get_container(int id);
class Item* get_item(ItemID id);
//class Item* get_or_create_item(int item_type, ItemID item_id);
int get_item_type(ItemID id);
int get_stack_size(ItemID id);  // space used in a stack
int get_stack_space(ItemID id); // space left in a stack
int get_stack_max(int item_type);

void destroy_item(ItemID id);

void merge_item_stack(ItemID src, ItemID dest);
void merge_item_stack(ItemID src, ItemID dest, int amount);

}

/*
CLIENT
*/
#if DC_CLIENT
namespace Item
{
    
void open_container();
void close_container();

int get_event_container_id(int event_id);
class ItemContainerUI* get_container_ui(int container_id);

class Item* create_item(int item_type, ItemID item_id);

void mouse_right_click_handler(int id, int slot);
void mouse_left_click_handler(int id, int slot);

ItemID* get_container_contents(int container_id);
int get_sprite_index_for_id(ItemID item_id);
int get_sprite_index_for_type(int item_type);
int* get_container_ui_types(int container_id);
int* get_container_ui_stacks(int container_id);

}
#endif 



/*
SERVER
*/

#if DC_SERVER
namespace Item
{

ItemID split_item_stack(ItemID src, int amount);
ItemID split_item_stack_in_half(ItemID src);
bool agent_owns_container(int agent_id, int container_id);

ItemID get_agent_hand(int agent_id);
int get_agent_container(int agent_id);
void assign_container_to_agent(int agent_id, int client_id);

class Item* create_item(int item_type);
class Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz);

void check_item_pickups();
void throw_item(int agent_id, ItemID item_id);

}
#endif

