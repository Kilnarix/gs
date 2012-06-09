#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/agent.hpp>
#include <agent/agent_status.hpp>
#include <agent/client/player_agent_action.hpp>

enum active_camera_states
{
    net_agent = 0,
    client_side_prediction_interpolated,
    client_side_prediction,
    last_server_snapshot,
    CameraStatesEnd
};

class PlayerAgent_state
{
    private:
        uint16_t sanitize_control_state(uint16_t cs);
        uint16_t pack_control_state(
            int f, int b, int l, int r,
            int jet, int jump, int crouch, int boost,
            int misc1, int misc2, int misc3
        );

        void set_control_state(uint16_t cs, float theta, float phi);

    public:

        //client side state variables
        bool crouching;     //move client side
        int jetpack_ticks;
        int jetpack_decay;

        //use for interpolated client side prediction
        class AgentState s0;
        class AgentState s1;

        //cameras
        //class AgentState s;   use s1              //client side predicted from control state
        class AgentState c;                 //client side prediction with interpolation
        class AgentState state_snapshot;    //last snapshot from server
        //camera update functions   
        void update_client_side_prediction_interpolated();
        //camera mode
        int camera_mode;
        AgentState camera_state;    //USE THIS FOR CAMERA!!!

        void toggle_camera_mode();
        void pump_camera();

        void display_agent_names();

        //control state history buffer
        int cs_seq_local;   // client side cs
        int cs_seq_net;     // snapshot cs sequence

        struct Agent_control_state cs_local[128];
        struct Agent_control_state cs_net[128];

        class AgentState snapshot_local[128];

        int state_history_seq;

        class AgentState* state_history;

        int state_history_index;
        int last_snapshot_time;

        void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);
        void handle_net_control_state(int _seq, int _cs, float _theta, float _phi);

        //state variables

        int agent_id;   //agent_id for player agent
        Agent_state* you;
        void set_PlayerAgent_id(int id);

        //set also sends
        void set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi);

        float camera_z();
        float camera_height();
        Vec3 camera_position();

        Vec3 get_weapon_fire_animation_origin()
        {
            Vec3 origin = this->camera_position();
            origin.z -= 0.4f;
            return origin;
        }

        void update_model();

        PlayerAgent_action action;

        bool identified;

        void was_identified();

        int facing_container();

        void update_sound();

        PlayerAgent_state();
        ~PlayerAgent_state();
};
