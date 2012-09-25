#pragma once

namespace Auth
{

void init();
void teardown();

void init_packets();

bool parse_auth_token(const char* token, int* user_id, time_t* expiration_time, char** hash, char** username);

bool auth_token_expired(const time_t timestamp, const time_t expiration_window);

bool is_valid_username(const char* username);
inline bool is_valid_user_id(const int user_id);

void run_tests();

void update();

}   // Auth
