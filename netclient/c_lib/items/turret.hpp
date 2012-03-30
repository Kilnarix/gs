#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/behaviour/behaviour.hpp>
#include <c_lib/entity/entity.hpp>

// forward decl
class object_shot_object_StoC;
class object_shot_terrain_StoC;
class object_shot_nothing_StoC;

void turret_shot_object(object_shot_object_StoC* msg);
void turret_shot_terrain(object_shot_terrain_StoC* msg);
void turret_shot_nothing(object_shot_nothing_StoC* msg);

extern VoxDat turret_vox_dat;

class Turret
{
    private:
        unsigned int fire_limiter;
        int team;
        int owner;
    public:
        int id;
        int health;
        Object_types type;

        float x,y,z;
        float theta,phi;
        float camera_height; // z offset to fire from
        
        Voxel_model* vox;
        void init_vox();
        int get_team();
        void set_team(int team);
        int get_owner();
        void set_owner(int owner);
        
        void set_position(float x, float y, float z);
        
        int get_coins_for_kill(int owner, int team);    // REMAINING
        int take_damage(int dmg);
        void acquire_target();

        void tick();
        void update(); // client side
        
        #ifdef DC_SERVER
        void create_message(object_create_owner_team_StoC* msg);   // REMAINING
        #endif
        explicit Turret(int id);
        Turret(int id, float x, float y, float z);
        ~Turret();
};

/* TargetAcquistion component */

class TargetAcquisitionComponent
{
    public:
    void acquire_target(ObjectState* state);
    TargetAcquisitionComponent(){}
};

class TurretObject; // forward decl

typedef TickStayOnGround < TickTargetAcquisition < NoTick(TurretObject), TurretObject>,TurretObject>
    TurretTick;

typedef NoDraw(TurretObject) TurretDraw;

typedef UpdateFrozenVox < NoUpdate(TurretObject), TurretObject> TurretUpdate;

typedef BornTeamVox < BornSetVox < BornUpdateFrozenVox < NoBorn(TurretObject), TurretObject>,TurretObject>,TurretObject>
    TurretBorn;

typedef DieExplode < NoDie(TurretObject), TurretObject> TurretDie;

typedef ObjectPolicy
<TurretObject, TurretTick, TurretDraw, TurretUpdate, TurretBorn, TurretDie, create_object_message, object_state_message >
TurretObjectParent;

class TurretObject: public TurretObjectParent, public TargetAcquisitionComponent
{
    public:
        TurretObject(int id, float x, float y, float z, float mx, float my, float mz)
        : TurretObjectParent(this), TargetAcquisitionComponent()
        {
            this->_state.id = id;
            this->_state.create_particle(x,y,z,mx,my,mz);
            //TODO
        }
};

#include <c_lib/template/object_list.hpp>
class Turret_list: public Object_list<Turret,MAX_TURRETS>
{
    private:
        const char* name() { return "Turret"; }

    public:
    
        bool point_occupied(int x, int y, int z);
        #ifdef DC_SERVER
        void send_to_client(int client_id);
        #endif

        void tick();
        void update();  // client side
        void alter_owner(int owner, int new_owner);

        Turret_list() { print(); }
};

