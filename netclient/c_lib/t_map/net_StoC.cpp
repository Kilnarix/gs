#include "net_StoC.hpp"

#ifdef DC_CLIENT
#include <c_lib/animations/animations.hpp>
#include <c_lib/common/random.h>

using namespace t_map;

void send_map_chunk(int x, int y, char* buffer, int n) {}

void handle_map_chunk(int x, int y, char *buffer, int n)
{
    
}

void chunk_meta_data_StoC::handle()
{
    //int chunk_x,chunk_y;
    //int version;
    printf("chunk_meta_data_StoC: chunk_x= %i chunk_y= %i version= %i \n", chunk_x, chunk_y, version);
}

void block_StoC::handle() 
{
    if (val == 0) 
    {
        int cube_id = _get(x,y,z);
        Animations::block_crumble((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, randrange(10,30), cube_id);
        // play sound
        char soundfile[] = "block_crumble.wav";
        Sound::play_3d_sound(soundfile, (float)x+0.5f, (float)y+0.5f, (float)z+0.5f, 0,0,0);
    }
    _set(x,y,z,val);
}

void map_metadata_StoC::handle() 
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
}


#endif


#ifdef DC_SERVER

void chunk_meta_data_StoC::handle() {}
void block_StoC::handle() {}
void map_metadata_StoC::handle() {}

void handle_map_chunk(int x, int y, char *buffer, int n) {}

void send_map_chunk(int x, int y, char* buffer, int n)
{
    
}

#endif
