#pragma once

#ifdef DC_CLIENT

#include <c_lib/agent/net_agent.cpp>
#define AGENT_STATE_HISTORY_SIZE 5
#define AGENT_INTERPOLATION_DECAY 0.5f

/*
    Use for client side and state update to server

*/

class PlayerAgent_state {
    private:
        class AgentState state_history[AGENT_STATE_HISTORY_SIZE];
        int state_history_index;
        
    public:
        float camera_height_scale;
        int agent_id;
        int seq;

        //last agent state update
        Agent_cs_CtoS cs_0; //last control state

        void calculate_interpolate();
        AgentState interpolate;

        void set_PlayerAgent_id(int id);
        //set actually sends
        void set_control_state(uint16_t cs, float theta, float phi);

        PlayerAgent_state() {
            static int inited=0;
            if (inited) printf("WARNING Only one PlayerAgent_state should exist\n");
            state_history_index = 0;
            agent_id = -1;
            seq = 0;
            camera_height_scale = 0.83f;
            inited++;
        }

        ~PlayerAgent_state() {
        }

};


#endif
