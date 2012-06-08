#pragma once

class NetPeerManager
{
    public:
        int client_id;
        bool inited;
        bool loaded;
        bool received_initial_state;

        time_t connection_time;

        void init(int client_id);
        void version_passed(int client_id);
        void ready();
        void send_remaining_state();
        void teardown();

    ~NetPeerManager();
    NetPeerManager();
};
