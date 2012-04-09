#pragma once

#include <net_lib/net.hpp>

//#include <net_lib/t_item/net/CtoSs.hpp>
//#include <c_lib/t_item/free_item.hpp>

namespace t_item
{

/*
    Free Item
*/
class free_item_picked_up_StoC: public FixedSizeReliableNetPacketToClient<free_item_picked_up_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        uint8_t agent_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

class free_item_create_StoC: public FixedSizeReliableNetPacketToClient<free_item_create_StoC>
{
    public:
        uint8_t type;
        uint16_t id;
        float x,y,z;
        float mx,my,mz;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
        }
        inline void handle();
};

class free_item_destroy_StoC: public FixedSizeReliableNetPacketToClient<free_item_destroy_StoC>
{
    public:
        uint8_t type;
        uint16_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};

/*
    Inventory
*/

class assign_agent_inventory_StoC: public FixedSizeReliableNetPacketToClient<assign_agent_inventory_StoC>
{
    public:
        uint8_t agent_id;
        uint16_t inventory_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent_id, buff, buff_n, pack);
            pack_u16(&inventory_id, buff, buff_n, pack);
        }
        inline void handle()
        {
            #ifdef DC_CLIENT
            
            
            #endif
        }
};


}