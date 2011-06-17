#include "draw_functions.h"

//struct line_vertices
int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1) {

//printf("(%i, %i, %i)\n", r,g,b);

glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
glBegin(GL_LINES);
glVertex3f(x0,y0,z0); // origin of the line
glVertex3f(x1,y1,z1); // ending point of the line
glEnd();
return 0;
}

int _draw_point(int r, int g,int b, float x0, float y0, float z0) {
glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
glBegin(GL_POINTS);
glVertex3f(x0,y0,z0); // point
glEnd();
return 0;
}

int _blit_sprite(int texture, float x0, float y0, float x1, float y1, float z) {

    glBindTexture( GL_TEXTURE_2D, texture );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

glBegin( GL_QUADS );
    glTexCoord2i( 0, 0 );
    glVertex3f( x0, y0, z );

    glTexCoord2i( 1, 0 );
    glVertex3f( x1, y0, z );

    glTexCoord2i( 1, 1 );
    glVertex3f( x1, y1, z );

    glTexCoord2i( 0, 1 );
    glVertex3f( x0, y1, z );
glEnd();

    glDisable(GL_BLEND);
}

///global
GLuint texture = 0;
SDL_Surface *surface;

GLuint VBOid = 0;

int vertex_num = 0;

int print_vertex(struct Vertex* v) {

printf("v_num: %i \n", vertex_num);
printf("x,y,z= %f, %f, %f\n", v->x,v->y,v->z);
printf("tx,ty= %f, %f\n", v->tx, v->ty);
printf("r,g,b,a= %i, %i, %i, %i\n", v->r,v->g,v->b,v->a);
printf("\n", v->r,v->g,v->b,v->a);
vertex_num +=1;
return 0;
}

int _bind_VBO(struct Quad* quad_list, int v_num) {

///test
if(1) {
    printf("v_num= %i \n", v_num);
    vertex_num = 0;
    int i,j;
    for(i=0; i<1; i++){
        for(j=0; j<4; j++){
        print_vertex(&quad_list[0].vertex[i]);
        }
    }
}
if(texture == 0) { //load texture if texture is not set
    surface=IMG_Load("texture/textures_01.png");
    if(!surface) {printf("IMG_Load: %s \n", IMG_GetError());return 0;}
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );



   // printf("binding\n");
    glBindTexture( GL_TEXTURE_2D, texture );

    glGenBuffers(1, &VBOid);
    glBindBuffer(GL_ARRAY_BUFFER, VBOid);
    glBufferData(GL_ARRAY_BUFFER, v_num*sizeof(struct Quad), quad_list, GL_STATIC_DRAW); // size, pointer to array, usecase
}

glEnable(GL_TEXTURE_2D);

glBindBuffer(GL_ARRAY_BUFFER, VBOid);

glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);

glClientActiveTexture(texture); //bind texture
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);

glEnableClientState(GL_COLOR_ARRAY);
glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);


//glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);
//glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12); //12 bytes in
//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);

glDrawArrays(GL_QUADS,0, v_num*4);

glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);

//printf("SDL_error: %s\n", SDL_GetError());
//    printf("finished\n");
///test
    return 0;
}
