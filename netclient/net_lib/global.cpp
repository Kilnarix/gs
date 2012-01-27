#include "global.hpp"

namespace NetClient
{
    NetPeer Server;
}

namespace NetServer
{
    NetPeer* pool[HARD_MAX_CONNECTIONS];

    Agent_state* agents[HARD_MAX_CONNECTIONS];
    void assign_agent_to_client(int client_id, Agent_state* a)
    {
        agents[client_id] = a;
    }
}
