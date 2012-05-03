#pragma once

#include <particle/billboard_text_hud.hpp>

class Agent_state;

typedef enum
{
    AGENT_VOX_IS_STANDING,
    AGENT_VOX_IS_CROUCHED,
    AGENT_VOX_IS_DEAD,
} AgentVoxStatus;

class Agent_event {
    private:
        Agent_state* a;
        unsigned char r,g,b;  // team colors
        bool first_time_receiving_coins;
        AgentVoxStatus vox_status;
        bool model_was_changed;
    public:

        class Particle::BillboardTextHud* bb;
        void display_name();
        void hide_name();

        // side effects of taking damage. dont modify health/death here
        void took_damage(int dmg);
        void healed(int health);
        void died();
        void born();
        void crouched();
        void uncrouched();
        bool model_changed();
        void set_agent_vox_status(AgentVoxStatus status);
        void life_changing(bool dead);
        void reload_weapon(int type);

        void joined_team(int team);
        void name_changed(char* old_name);
        void update_team_color(unsigned char r, unsigned char b, unsigned char c);
        void update_team_color();
        
        void picked_up_flag();
        void dropped_flag();
        void scored_flag();

        // new style weapon trigger events
        // only triggers agent specific animations/sounds
        void tick_mining_laser();   // continuous, while "on"
        void fired_mining_laser();  // when fire rate tick triggers

        // old style packet handler events
        // still used; triggers target specific animations/sounds
        // since that is required from server
        void fired_weapon_at_object(int id, int type, int part);
        void fired_weapon_at_block(float x, float y, float z, int cube, int side);
        void fired_weapon_at_nothing();
        void melee_attack_object(int id, int type, int part);
        void melee_attack_nothing();
        void fire_empty_weapon(int weapon_type);

        void hit_block();
        void placed_block();
        void threw_grenade();

        void set_spawner(int pt);
        void coins_changed(unsigned int coins);

        explicit Agent_event(Agent_state* owner);
        ~Agent_event();
};
