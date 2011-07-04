
cimport libc.stdlib

#type imports
cimport cube_lib.types
from cube_lib.types cimport Quad, Vertex

#cdef extern from "SDL.h":

## Camera.c ##
cdef extern struct Camera: #maybe public?
    float fov
    float x_size
    float y_size
    float z_near
    float z_far
    float x
    float y
    float z
    float x_angle
    float y_angle

cdef extern int _world_projection(Camera* camera)
cdef extern int _hud_projection(Camera* camera)

## End Camera.c ##

## Texture Loader ##

cdef extern from "SDL.h":
    struct SDL_Surface:
        int w
        int h

cdef extern from "texture_loader.h":
    int _init_image_loader()
    SDL_Surface* _load_image(char *file)
    int _create_hud_texture(char *file) #deprecate
    int _create_block_texture(char *file) #deprecate
    int _create_texture(SDL_Surface* surface)

cdef SDL_Surface* load_image(char* file):
    cdef SDL_Surface* surface
    surface = _load_image(file)
    return surface

cdef create_texture(SDL_Surface* surface, type =None): #eventually support mippapped textures
    return _create_texture(surface)

## SDL functions ##

cdef extern int _init_video()
cdef extern int _del_video()
cdef extern int _swap_buffers()
cdef extern int _get_ticks()

def get_ticks():
    return _get_ticks()

## Draw functions ##

#cdef extern struct Quad
#cdef extern struct Vertex

cdef extern from "draw_functions.h":
    int _draw_point(int r, int g,int b, float x0, float y0, float z0)
    int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1)
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)
    #int _bind_VBO(Quad* quad_list,int v_num)

def draw_line(int r, int g, int b, float x0, float y0, float z0, float x1, float y1, float z1):
    return _draw_line(r,g,b,x0,y0,z0,x1,y1,z1)

def draw_point(int r, int g, int b, float x0, float y0, float z0):
    return _draw_point(r,g,b,x0,y0,z0)

#deprecate
cdef bind_VBO(Quad* quad_list, int v_num):
    pass
    #_bind_VBO(quad_list, v_num)


## Window Properties ##

cdef class Window:
    cdef int w
    cdef int h

cdef class Texture:
    cdef int id
    cdef int w
    cdef int h
    cdef SDL_Surface* surface

    def __init__(Texture self, char * file):
        #print "init runs"
        self.surface = load_image(file)
        self.w = self.surface.w
        self.h = self.surface.h
        self.id = _create_texture(self.surface)

    def draw(self, x0, y0, x1, y1, z=-0.5):
        #print "id == " + str(self.id)
        #print "w,h = %i, %i" % (self.w, self.h)
        _blit_sprite(self.id, x0, y0, x1, y1, z)
        #_blit_sprite(1, x0, y0, x1, y1, z)

class Textures:
    hud_tex = None
    tile_tex = None

    def init(self):
        print "Initing Textures"
        self.hud_tex = Texture("./texture/target.png")
        self.tile_tex = Texture("./texture/textures_01.png")

from libc.stdlib cimport malloc, free

cdef class Global:
    cdef Camera* camera
    #textures = Textures()
#    cdef Window window

    #make field of view adjustable!
    def __init__(self):
        cdef Camera *camera = <Camera *>malloc(sizeof(Camera))
        self.camera = camera

        self.set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
        self.set_projection(0.,0.,0.,0.,0.)

    def init(self):
        print "Creating SDL OpenGL Window"
        _init_video()
        #_init_input() ##fix

        #_init_image_loader()
        #self.textures.hud_tex = Texture("./texture/target.png", 0)
        #self.textures.tile_tex = Texture("./texture/textures_01.png",0)
        #self.camera = Camera()

    def close_window(self):
        print "Deconstructing SDL OpenGL Window"
        _del_video()

    def flip(self):
        _swap_buffers()

    #camera
    def set_aspect(self, float fov, float x_size, float y_size, float z_near, float z_far):
        cdef Camera* camera
        camera = self.camera

        camera.fov = fov
        camera.x_size = x_size
        camera.y_size = y_size
        camera.z_near = z_near
        camera.z_far = z_far

    def set_projection(self, float x, float y, float z, float x_angle, float y_angle):
        cdef Camera* camera
        camera = self.camera

        camera.x = x
        camera.y = y
        camera.z = z
        camera.x_angle = x_angle
        camera.y_angle = y_angle

    def world_projection(self):
        _world_projection(self.camera)

    def hud_projection(self):
        _hud_projection(self.camera)

### init

SDL_global = Global()
