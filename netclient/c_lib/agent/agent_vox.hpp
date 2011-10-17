#pragma once

#include <math.h>

#include <compat.h>
#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

//#include <ray_trace/ray_trace.h>
//#include <t_map/t_map.h>
//#include <t_map/t_properties.h>

#include <physics/vector.h>

#define AGENT_PART_NUM 6
#define AGENT_PART_HEAD 0
#define AGENT_PART_TORSO 1
#define AGENT_PART_LARM 2
#define AGENT_PART_RARM 3
#define AGENT_PART_LLEG 4
#define AGENT_PART_RLEG 5

struct Voxel {
unsigned char r,g,b,a;
};

//struct Vox {

    //struct Vector f,n,u;
    //struct Vector a; //forward, normal, anchor

    //unsigned short xdim;
    //unsigned short ydim;
    //unsigned short zdim;
    //float vox_size;
    //float radius;
    //struct Voxel* vox;
    //unsigned int num_vox;

    //struct Vector c; //center
    //float length;
//};

class Vox {
    public:
        struct Vector f,n,u;
        struct Vector a; //forward, normal, anchor

        unsigned short xdim;
        unsigned short ydim;
        unsigned short zdim;
        float vox_size;
        float radius;
        struct Voxel* vox;
        unsigned int num_vox;

        struct Vector c; //center
        float length;
        Vox(unsigned short _xdim, unsigned short _ydim, unsigned short _zdim,
            float vosize, float r, unsigned int nv) {
            f = Vector_init(0.0f, 0.0f, 0.0f);
            n = Vector_init(0.0f, 0.0f, 0.0f);
            u = Vector_init(0.0f, 0.0f, 0.0f);
            a = Vector_init(0.0f, 0.0f, 0.0f);
            xdim = _xdim;
            ydim = _ydim;
            zdim = _zdim;
            vox_size = vosize;
            radius = r;
            vox = (struct Voxel*) malloc(sizeof(struct Voxel*));
            num_vox = nv;
        }
        ~Vox() {
            free(vox);
        }
};

class Agent_state;
class Agent_list;

class Agent_vox {
    public:
        float lv,ly,lz; //looking vector
        float camera_height;
        float cbox_height;
        float cbox_radius; // collision box
        class Vox* vox_part[AGENT_PART_NUM]; //head,torso, larm,rarm, lleg, rleg
        Agent_vox() {
            lv=ly=lz=0.0;
            camera_height=2.5;
            cbox_height=3.0;
            cbox_radius=0.45;
            for (int i=0; i<AGENT_PART_NUM; i++) {
                vox_part[i] = new Vox();
            }
        }
};

void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize);

void set_agent_limb_direction(int id, int part, float fx,float fy,float fz, float nx,float ny, float nz);
void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az);

void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a);

struct Vox* get_agent_vox_part(int id, int part);

void destroy_vox(struct Vox* v);

/*
 *  Client only
 */
#ifdef DC_CLIENT
void agent_vox_draw_head(struct Vector look, struct Vector right, Agent_state* a);
void agent_vox_draw_vox_volume(struct Vox* v, struct Vector right, Agent_state* a);
#endif
