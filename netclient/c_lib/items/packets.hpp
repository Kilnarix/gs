#pragma once

#include <net_lib/net.hpp>

// Position
class object_create_StoC_model
{
    public:
        uint8_t type;
        uint16_t id;
        uint8_t team;
        uint8_t owner;
        uint8_t team_index;
        float x,y,z;
        float mx,my,mz;
        
        inline virtual void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        virtual void handle();
};

// Concrete implementation
class object_create_StoC:
public FixedSizeReliableNetPacketToClient<object_create_StoC>, public object_create_StoC_model
{
    public:
    inline void packet(char* buff, int* buff_n, bool pack)
    {
        object_create_StoC_model::packet(buff, buff_n, pack);
    }
    inline void handle() { object_create_StoC_model::handle(); }
};

// Position + Velocity(momentum)
class object_create_momentum_StoC_model: public object_create_StoC_model
{
    public:
    inline virtual void packet(char* buff, int* buff_n, bool pack)
    {
        object_create_StoC_model::packet(buff, buff_n, pack);
        pack_float(&mx, buff, buff_n, pack);
        pack_float(&my, buff, buff_n, pack);
        pack_float(&mz, buff, buff_n, pack);
    }
    virtual void handle();
};

// Concrete implementation
class object_create_momentum_StoC:
public FixedSizeReliableNetPacketToClient<object_create_momentum_StoC>, public object_create_momentum_StoC_model
{
    public:
    inline void packet(char* buff, int* buff_n, bool pack)
    {
        object_create_momentum_StoC_model::packet(buff, buff_n, pack);
    }
    inline void handle() { object_create_momentum_StoC_model::handle(); }
};

// Position + owner,team
class object_create_owner_team_StoC_model: public object_create_StoC_model
{
    public:
        inline virtual void packet(char* buff, int* buff_n, bool pack)
        {
            object_create_StoC_model::packet(buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
            pack_u8(&team, buff, buff_n, pack);
        }
        virtual void handle();
};

// Concrete implementation
class object_create_owner_team_StoC:
public FixedSizeReliableNetPacketToClient<object_create_owner_team_StoC>, public object_create_owner_team_StoC_model
{
    public:
    inline void packet(char* buff, int* buff_n, bool pack)
    {
        object_create_owner_team_StoC_model::packet(buff, buff_n, pack);
    }
    inline void handle() { object_create_owner_team_StoC_model::handle(); }
};

// Position + owner,team,team_index
class object_create_owner_team_index_StoC_model: public object_create_owner_team_StoC_model
{
    public:
        inline virtual void packet(char* buff, int* buff_n, bool pack)
        {
            object_create_owner_team_StoC_model::packet(buff, buff_n, pack);
            pack_u8(&team_index, buff, buff_n, pack);
        }
        virtual void handle();
};

// Concrete implementation
class object_create_owner_team_index_StoC:
public FixedSizeReliableNetPacketToClient<object_create_owner_team_index_StoC>, public object_create_owner_team_index_StoC_model
{
    public:
    inline void packet(char* buff, int* buff_n, bool pack)
    {
        object_create_owner_team_index_StoC_model::packet(buff, buff_n, pack);
    }
    inline void handle() { object_create_owner_team_index_StoC_model::handle(); }
};

/* Destruction */

class object_destroy_StoC: public FixedSizeReliableNetPacketToClient<object_destroy_StoC>
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

/* State */

class object_state_StoC_model
{
    public:
        uint8_t id;
        uint8_t type;
        float x,y,z;
        float mx,my,mz;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        void handle();
};

// concrete
class object_state_StoC:
public object_state_StoC_model, public FixedSizeReliableNetPacketToClient<object_state_StoC>
{
    public:
    inline virtual void packet(char* buff, int* buff_n, bool pack)
    {
        object_state_StoC_model::packet(buff, buff_n, pack);
    }
    inline virtual void handle() { object_state_StoC_model::handle(); }
};

class object_state_momentum_StoC_model: public object_state_StoC_model
{
    public:
        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            object_state_StoC_model::packet(buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
        }
        void handle();
};

// concrete
class object_state_momentum_StoC:
public object_state_momentum_StoC_model, public FixedSizeReliableNetPacketToClient<object_state_momentum_StoC>
{
    public:
    inline virtual void packet(char* buff, int* buff_n, bool pack)
    {
        object_state_momentum_StoC_model::packet(buff, buff_n, pack);
    }
    inline virtual void handle() { object_state_momentum_StoC_model::handle(); }
};

/* Actions */

class object_picked_up_StoC: public FixedSizeReliableNetPacketToClient<object_picked_up_StoC>
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

class object_shot_object_StoC: public FixedSizeNetPacketToClient<object_shot_object_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t target_id;
        uint8_t target_type;
        uint8_t target_part;
        uint8_t voxel_x;
        uint8_t voxel_y;
        uint8_t voxel_z;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            pack_u8(&voxel_x, buff, buff_n, pack);
            pack_u8(&voxel_y, buff, buff_n, pack);
            pack_u8(&voxel_z, buff, buff_n, pack);
        }
    inline void handle();
};

class object_shot_terrain_StoC: public FixedSizeNetPacketToClient<object_shot_terrain_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t cube;
        uint8_t side;
        float x,y,z;

    inline void packet(char* buffer, int* buff_n, bool pack)
    {
        pack_u16(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_u8(&cube, buffer, buff_n, pack);
        pack_u8(&side, buffer, buff_n, pack);
        pack_float(&x, buffer, buff_n, pack);
        pack_float(&y, buffer, buff_n, pack);
        pack_float(&z, buffer, buff_n, pack);
    }
    inline void handle();
};

class object_shot_nothing_StoC: public FixedSizeNetPacketToClient<object_shot_nothing_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        float x,y,z;

    inline void packet(char* buffer, int* buff_n, bool pack)
    {
        pack_u16(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_float(&x, buffer, buff_n, pack);
        pack_float(&y, buffer, buff_n, pack);
        pack_float(&z, buffer, buff_n, pack);
    }
    inline void handle();
};

/* CreateMessage handlers */
// TODO -- Move these

// NOTE: all must have the same arguments except the first argument (msg) type

#include <c_lib/entity/state.hpp>

/* Create */

void create_message(object_create_StoC* msg, int id, Object_types type, Vec3 position, Vec3 momentum, int owner, int team, int team_index)
{
    msg->id = id;
    msg->type = type;
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
}

void create_message(object_create_momentum_StoC* msg, int id, Object_types type, Vec3 position, Vec3 momentum, int owner, int team, int team_index)
{
    msg->id = id;
    msg->type = type;
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;
}

void create_message(object_create_owner_team_StoC* msg, int id, Object_types type, Vec3 position, Vec3 momentum, int owner, int team, int team_index)
{
    msg->id = id;
    msg->type = type;
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
    msg->owner = owner;
    msg->team = team;
}

void create_message(object_create_owner_team_index_StoC* msg, int id, Object_types type, Vec3 position, Vec3 momentum, int owner, int team, int team_index)
{
    msg->id = id;
    msg->type = type;
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
    msg->owner = owner;
    msg->team = team;
    msg->team_index = team_index;
}

/* State */

void state_message(object_state_StoC* msg, int id, Object_types type, Vec3 position, Vec3 momentum)
{
    msg->id = id;
    msg->type = type;
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
}

void state_message(object_state_momentum_StoC* msg, int id, Object_types type, Vec3 position, Vec3 momentum)
{
    msg->id = id;
    msg->type = type;
    msg->x = position.x;
    msg->y = position.y;
    msg->z = position.z;
    msg->mx = momentum.x;
    msg->my = momentum.y;
    msg->mz = momentum.z;
}
