#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/behaviour/behaviour.hpp>
#include <c_lib/entity/entity.hpp>

//forward decl
class SpawnerList;

#if DC_SERVER
namespace ServerState
{
    extern SpawnerList* spawner_list;
}
#endif
#if DC_CLIENT
namespace ClientState
{
    extern SpawnerList* spawner_list;
}
#endif

extern VoxDat spawner_vox_dat;

/* Spawner Component */

class SpawnerProperties
{
    public:
    int radius;
    ObjectPolicyInterface* obj;
    SpawnerList* list;
    
    SpawnerProperties()
    : radius(1), obj(NULL), list(NULL)
    {}
};

class SpawnerComponent
{
    public:
        SpawnerProperties spawner_properties;
        void get_spawn_point(ObjectState* state, int spawned_object_height, int* spawn_pt);
    SpawnerComponent(){}
};

/* List management */

const int SPAWNER_LIST_MAX = 256;
class SpawnerList
{
    public:
    SpawnerProperties** spawners;
    int max;
    int max_per_team;
    int ct;

    void register_object(SpawnerProperties* state)
    {
        if (this->ct >= this->max)
        {
            printf("WARNING: SpawnerList is full\n");
            return;
        }
        int i=0;
        for (;i<this->max; i++)
        {
            if (this->spawners[i] == NULL)
            {
                state->list = this;
                this->spawners[i] = state;
                this->ct++;
                break;
            }
        }
        if (i == this->max)
            printf("WARNING: no empty slots found in spawner list\n");
    }

    void unregister_object(SpawnerProperties* state)
    {
        for (int i=0; i<this->max; i++)
        {
            if (this->spawners[i] == NULL) continue;
            if (this->spawners[i] == state)
            {
                state->list = NULL;
                this->ct--;
                this->spawners[i] = NULL;
                return;
            }
        }
    }

    /* MAJOR TODO -- MAKE THIS ITS OWN LIST */
    bool team_spawner_available(int team);
    int get_random_spawner(int team);
    int get_numbered_team_spawner(int team, int id);
    ObjectPolicyInterface* get_by_team_index(int team, int team_index);
    bool spawner_exists(int team, int team_index);
    void assign_team_index(ObjectPolicyInterface* spawner);
    /* END SHIT */

    ~SpawnerList()
    {
        free(this->spawners);
    }
    SpawnerList()
    : max(SPAWNER_LIST_MAX), max_per_team(SPAWNERS_PER_TEAM), ct(0)
    {
        this->spawners = (SpawnerProperties**)calloc(this->max, sizeof(SpawnerProperties*));
    }
};

typedef ObjectInterface
< object_create_owner_team_index_StoC, object_state_StoC >
SpawnerInterface;

class Spawner: public SpawnerComponent, public SpawnerInterface
{
    public:
    Spawner(int id)
    : SpawnerComponent(), SpawnerInterface()
    {
        this->_state.id = id;
        this->_state.height = SPAWNER_HEALTH;
        this->_state.broadcast_state_change = true;
        this->_state.cost = COST_SPAWNER;
        this->_state.reward = COST_SPAWNER;
        this->_state.coin_rule = COINS_ENEMIES | COINS_OWNER;
        this->_state.frozen_vox = true;
        this->_state.vox_dat = &spawner_vox_dat;
        this->_state.type = OBJ_TYPE_SPAWNER;
        this->_state.health = SPAWNER_HEALTH;

        this->spawner_properties.obj = this;
        this->spawner_properties.radius = SPAWNER_SPAWN_RADIUS;
        STATE::spawner_list->register_object(&this->spawner_properties);
    }

    ~Spawner()
    {
        STATE::spawner_list->unregister_object(&this->spawner_properties);
    }

    /* Interface */

    void tick()
    {
        tickStayOnGround(this->state(), this);
    }

    void update()
    {
        updateFrozenVox(this->state(), this);
    }

    void draw() {}

    void born()
    {
        bornTeamVox(this->state(), this);
        bornSetVox(this->state(), this);
        bornUpdateFrozenVox(this->state(), this);
        bornCreateMessage(this->state(), this);
    }

    void die()
    {
        dieBroadcast(this->state(), this);
        dieRevokeOwner(this->state(), this);
        dieTeamItemAnimation(this->state(), this);
    }
};

