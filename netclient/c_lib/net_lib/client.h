#pragma once

#ifdef DC_CLIENT

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <time/physics_timer.h>

//message handling
//#include <agent/control_state.h>
//#include <agent/agent_client.h>

/*
 #ifdef __cplusplus
 extern "C" {
 #endif

 #ifdef __cplusplus
 }
 #endif
*/

void _NetClientConnect(int a, int b,int c, int d, int _port);
//void _NetClientTick();


void _NetClientStateTick();
void _NetClientNetInTick();
void _NetClientNetOutTick();

#endif
