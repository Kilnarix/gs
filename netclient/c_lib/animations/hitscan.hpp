#pragma once

#include <c_lib/template/object_list.hpp>

#include <c_lib/camera/camera.hpp>

#include <compat_gl.h>
#include <c_lib/SDL/texture_loader.h>
#include <c_lib/physics/vector.hpp>

namespace Animations
{

static int hitscan_texture_id;

void init_hitscan()
{
    int i;
    i = create_texture_from_file( (char*) "./media/texture/hitscan/hitscan_01.png", &hitscan_texture_id);
    if (i) { printf("init_hitscan failed with code %d\n", i); }
}


class HitscanEffect
{
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    int ttl;

    void draw();
    void tick();

    HitscanEffect() { ttl = 3000; }
};

void HitscanEffect::draw()
{

    const float width = 1.0;
    const float height = 4.0;


    //for billboarding
    float cx = current_camera->x;
    float cy = current_camera->y;
    float cz = current_camera->z;


    struct Vector c = Vector_init(current_camera->x, current_camera->y, current_camera->z);

    struct Vector r = Vector_init(vx,vy,vz);
    normalize_vector( &r );
    //vector_scalar2(&r)

    struct Vector x1 = Vector_init(x,y,z);
    struct Vector l1 = sub_vec(&x1, &c);

    struct Vector u1 = vector_cross( l1, r);
    normalize_vector( &u1 );

    struct Vector x2 = Vector_init(x- height*vx, y - height*vy, z - height*vz);
    struct Vector l2 = sub_vec(&x2, &c);

    struct Vector u2 = vector_cross( l2, r);
    normalize_vector( &u2 );


    Vector top_left = Vector_init(x1.x + width*u1.x, x1.y + width*u1.y, x1.z + width*u1.z);
    Vector top_right = Vector_init(x1.x - width*u1.x, x1.y - width*u1.y, x1.z - width*u1.z);

    Vector bottom_right = Vector_init(x2.x - width*u2.x, x2.y - width*u2.y, x2.z - width*u2.z);
    Vector bottom_left = Vector_init(x2.x + width*u2.x, x2.y + width*u2.y, x2.z + width*u2.z);

    const float tx_min = 0.0;
    const float tx_max = 1.0;
    const float ty_min = 0.0;
    const float ty_max = 1.0;

    glTexCoord2f(tx_min,ty_max );
    glVertex3f( x2.x + width*u2.x, x2.y + width*u2.y, x2.z + width*u2.z );  // Bottom left

    glTexCoord2f(tx_min,ty_min );
    glVertex3f( x1.x + width*u1.x, x1.y + width*u1.y, x1.z + width*u1.z);  // Top left

    glTexCoord2f(tx_max,ty_min);
    glVertex3f( x1.x - width*u1.x, x1.y - width*u1.y, x1.z - width*u1.z );  // Top right

    glTexCoord2f(tx_max,ty_max );
    glVertex3f( x2.x - width*u2.x, x2.y - width*u2.y, x2.z - width*u2.z );  // Bottom right

}

void HitscanEffect::tick()
{
    x += vx /30.0;
    y += vy /30.0;
    z += vz /30.0;    

    //check for collision with terrain/players
    //play animations for terrain/player collision
}

class HitscanEffect_list: public Object_list<HitscanEffect, MINIVOX_MAX>
{
    private:
        const char* name() { return "HitscanEffect"; }
    public:
        void draw();
        void tick();

        HitscanEffect_list() {}
};


void HitscanEffect_list::draw()
{
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, hitscan_texture_id );
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );

    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->draw();
    }

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);


}

void create_hitscan(float x,float y,float z, float vx, float vy, float vz);

#include <stdlib.h>

void HitscanEffect_list::tick()
{
    const float vm = 4.0;
    float vx = vm*(float)rand()/(float)RAND_MAX;
    float vy = vm*(float)rand()/(float)RAND_MAX;
    float vz = vm*(float)rand()/(float)RAND_MAX;

    create_hitscan(32.0, 32.0, 64.0, vx, vy, vz);

    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        a[i]->ttl--;
        if(a[i]->ttl == 0) destroy(a[i]->id);
    }
}

HitscanEffect_list hitscanEffect_list;

void create_hitscan(float x,float y,float z, float vx, float vy, float vz)
{
    HitscanEffect* he = hitscanEffect_list.create_imp_id();

    if(he == NULL) return;

    he->x=x; he->y=y; he->z=z;
    he->vx=vx; he->vy=vy; he->vz=vz;
   
    //printf("Hitscan created: id= %i \n", he->id);
}

}
