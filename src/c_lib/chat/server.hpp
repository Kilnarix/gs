#pragma once

#include <chat/globals.hpp>

class ChatServerChannel
{
    public:
    int id;
    ClientID* listeners;
    int n;
    int max;
    
    void broadcast(ClientID sender, char* payload);
    bool add_listener(ClientID id);
    bool remove_listener(ClientID id);

    explicit ChatServerChannel(int max);
    ~ChatServerChannel();
};

class ChatServer
{
    public:
    ChatServerChannel* system;
    ChatServerChannel* global;
    ChatServerChannel** pm;

    int channels[CHAT_SERVER_CHANNELS_MAX];

    FILE* logfile;
    unsigned int log_msg_buffer_len;
    char* log_msg_buffer;

    void player_join(ClientID id);
    void player_quit(ClientID id);
    void receive_message(int channel, ClientID sender, char* payload);
    void log_message(int channel, ClientID sender, char* payload);
    
    ChatServer();
    ~ChatServer();
};
