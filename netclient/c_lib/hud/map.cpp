#include "map.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.h>

namespace HudMap
{

static int num_cells = 0;
static unsigned char* cells = NULL;

// for texture init
static const int width = 512;
static const int height = 512;
static const int screen_x_offset = 50;
static const int screen_y_offset = 50;

static SDL_Surface* map_surface = NULL;
static GLuint map_textures[2];

static SDL_Surface* overlay_surface = NULL;
static GLuint overlay_textures[2];

static SDL_Surface* gradient_surface = NULL;

// COLORING ICONS:
// load surface
// swap pixels from base to new
// generate texture
// free surface
void load_colored_icon(
    char* fn, GLuint* texture,
    unsigned char br, unsigned char bg, unsigned char bb,   // base color
    unsigned char r, unsigned char g, unsigned char b       // replace with
)
{
    // format path
    const char icon_path_fmt[] = "./media/texture/icons/%s";
    char* path = (char*)calloc(strlen(icon_path_fmt) + strlen(fn) - 2 + 1, sizeof(char));
    sprintf(path, icon_path_fmt, fn);

    // get surface
    SDL_Surface* s = NULL;
    s = create_surface_from_file(path);
    if (s==NULL)
    {
        printf("ERROR: Failed to create surface for %s\n", path);
        return;
    }
    SDL_LockSurface(s);
    glEnable(GL_TEXTURE_2D);

    // alter surface
    Uint32 pix;
    Uint8 sr,sg,sb,sa;
    for (int i=0; i<s->w*s->h; i++)
    {
        pix = ((Uint32*)s->pixels)[i];
        SDL_GetRGBA(pix, s->format, &sr, &sg, &sb, &sa);
        if (sr == br && sg == bg && sb == bb)
            ((Uint32*)s->pixels)[i] = SDL_MapRGBA(s->format, r,g,b,sa);
    }
    glDisable(GL_TEXTURE_2D);

    // create texture
    if (create_texture_from_surface(s, texture))
        printf("ERROR: failed to create texture from surface for %s\n", path);

    // cleanup
    SDL_UnlockSurface(s);
    SDL_FreeSurface(s);
    free(path);
}

/* Map icons */
static GLuint spawner_icons[N_TEAMS] = {0};
static GLuint base_icons[N_TEAMS] = {0};
static GLuint flag_icons[N_TEAMS] = {0};
void update_team_icons(int team)    // team is optional; team=0 means generate all
{
    char spawner_icon_filename[] = "spawner_map_icon2.png";
    char base_icon_filename[] = "base_icon.png";
    char flag_icon_filename[] = "flag_icon.png";

    // pack filenames and icon arrays for easy looping
    int i=0;
    const int N_ICONS = 3;
    char* icon_filenames[N_ICONS];
    icon_filenames[i++] = spawner_icon_filename;
    icon_filenames[i++] = base_icon_filename;
    icon_filenames[i++] = flag_icon_filename;
    i=0;
    GLuint* icons[N_ICONS];
    icons[i++] = spawner_icons;
    icons[i++] = base_icons;
    icons[i++] = flag_icons;

    // base color, to replace
    unsigned char br = 0;
    unsigned char bg = 0;
    unsigned char bb = 0;

    if (!team)
        for (int i=0; i<(int)N_TEAMS; i++)
        {
            unsigned char r,g,b;    // color to replace with
            ClientState::ctf->get_team_color(i+1, &r, &g, &b);
            for (int j=0; j<N_ICONS; j++)
                load_colored_icon(icon_filenames[j], &icons[j][i], br,bg,bb, r,g,b);
        }
    else
    {
        unsigned char r,g,b;    // color to replace with
        ClientState::ctf->get_team_color(team, &r, &g, &b);
        for (int j=0; j<N_ICONS; j++)
            load_colored_icon(icon_filenames[j], &icons[j][team-1], br,bg,bb, r,g,b);
    }

}

// create blank surface
void init_surface()
{
    printf("init: hud_map \n");

    const int grad_num = 4;
    const char grad_fmt[] = "media/texture/heightmap_gradient_%02d.png";
    char grad_str[strlen(grad_fmt) -2 +1];
    sprintf(grad_str, grad_fmt, grad_num);
    gradient_surface = create_surface_from_file(grad_str);
    if (gradient_surface==NULL)
    {
        printf("HudMap gradient surface is NULL\n");
        return;
    }

    Uint32 tex_format;

    /* Init blank map surface */
    map_surface = create_surface_from_nothing(width, height);
    if (map_surface==NULL)
    {
        printf("HudMap blank surface is NULL\n");
        return;
    }

    tex_format = GL_BGRA;
    if (map_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;
    
    // texture
    glEnable(GL_TEXTURE_2D);
    for (int i=0; i<2; i++)
    {
        glGenTextures(1, &map_textures[i]);
        glBindTexture(GL_TEXTURE_2D, map_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_BGRA
        glTexImage2D( GL_TEXTURE_2D, 0, 4, map_surface->w, map_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, map_surface->pixels );
    }
    glDisable(GL_TEXTURE_2D);

    /* Init blank indicator overlay surface */
    overlay_surface = create_surface_from_nothing(width, height);
    if (overlay_surface==NULL)
    {
        printf("Hud indicator overlay blank surface is NULL\n");
        return;
    }

    tex_format = GL_BGRA;
    if (overlay_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    // texture
    glEnable(GL_TEXTURE_2D);
    for (int i=0; i<2; i++)
    {
        glGenTextures(1, &overlay_textures[i]);
        glBindTexture(GL_TEXTURE_2D, overlay_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_BGRA
        glTexImage2D( GL_TEXTURE_2D, 0, 4, overlay_surface->w, overlay_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, overlay_surface->pixels );
    }
    glDisable(GL_TEXTURE_2D);

    update_team_icons();
}

void init_cells()
{
    int n_cells = map_dim.x * map_dim.y;
    if (n_cells == num_cells) return;   // size same, dont change

    num_cells = n_cells;

    if (!n_cells)
    { // no cells, free/null cells
        free(cells);
        cells = NULL;
        return;
    }

    if (cells != NULL)
        cells = (unsigned char*)realloc(cells, num_cells*sizeof(unsigned char));
    else
        cells = (unsigned char*)calloc(num_cells, sizeof(unsigned char));
}

void init()
{
    init_surface();
    init_cells();
}

void update_heightmap()
{
    static int strip = 0;
    const int strips = 16*8;
    int strip_width = map_dim.x / strips;
    
    if (cells == NULL) return;
    int i,j;
    int h;
    for (i=strip_width*strip; i < strip_width*(strip+1); i++)
        for (j=0; j < map_dim.y; j++)
        {
            h = get_height_at(i,j);
            cells[i + map_dim.x*j] = (unsigned char)2*h;
        }

    strip++;
    strip%=strips;
}

void update_map_surface()
{
    if (map_surface == NULL) return;
    if (cells == NULL) return;
    SDL_LockSurface(map_surface);
    
    Uint32 pix;
    Uint8 r,g,b,a;
    for (int i=0; i<num_cells; i++)
    {
        pix = ((Uint32*)gradient_surface->pixels)[cells[i]];
        SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
        ((Uint32*)map_surface->pixels)[i] = SDL_MapRGBA(map_surface->format, b,g,r,a);
    }

    SDL_UnlockSurface(map_surface);
}

void update_texture(GLuint texture, SDL_Surface* surface)
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, map_dim.x, map_dim.y, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

    glDisable(GL_TEXTURE_2D);
}

void update_overlay_surface()
{
    // blank the surface
    for (int i=0; i<num_cells; i++)
        ((Uint32*)overlay_surface->pixels)[i] = SDL_MapRGBA(overlay_surface->format, 0,0,0,0);
}

void draw_2x2_pixel(Uint32 pix, int x, int y)
{
    int j=0,k=0;
    for (int i=0; i<4; i++)
    {
        if (i==2)
            j = 1;
        if (i==3)
            k = 1;
        
        if (x+j >= 0 && x+j < width && y+k >= 0 && y+k < height)    // only draw in surface bounds (or could segfault)
        {
            ((Uint32*)overlay_surface->pixels)[x+j + map_dim.x*(y+k)] = pix;
        }
    }
}

void update_agents()
{
    if (ClientState::playerAgent_state.you == NULL) return;
    int team = ClientState::playerAgent_state.you->status.team;
    if (team == 0) return;

    int x,y;

    x = (int)ClientState::playerAgent_state.camera_state.x;
    y = (int)ClientState::playerAgent_state.camera_state.y;
    
    // set agent pixel
    Uint8 r = 210,
          g = 150,
          b = 10,
          a = 255;
    Uint32 pix = SDL_MapRGBA(overlay_surface->format, b,g,r,a); // bgra, red
    draw_2x2_pixel(pix,x,y);

    // draw teammates
    ClientState::ctf->get_team_color(team, &r, &g, &b);
    for (int i=0; i<ClientState::agent_list->n_max; i++)
    {
        Agent_state* agent = ClientState::agent_list->a[i];
        if (agent == NULL) continue;
        if (agent->status.team != team) continue;
        if (agent->id == ClientState::playerAgent_state.agent_id) continue;

        x = (int)agent->s.x;
        y = (int)agent->s.y;

        pix = SDL_MapRGBA(overlay_surface->format, b, g, r, a);
        draw_2x2_pixel(pix,x,y);
    }
}

void update_indicators(int tex_id)
{

    if (overlay_surface == NULL) return;
    SDL_LockSurface(overlay_surface);

    update_overlay_surface();
    update_agents();

    SDL_UnlockSurface(overlay_surface);
    
    update_texture(overlay_textures[tex_id], overlay_surface);
}

void update_terrain_map(int tex_id)
{
    init_cells();   // updates cells array if map size changed
    if (cells == NULL) return;
    update_heightmap();
    update_map_surface();
    update_texture(map_textures[tex_id], map_surface);
}

void world_to_map_screen_coordinates(float x, float y, float *sx, float *sy)
{
    float x_scale = ((float)width)/((float)map_dim.x);
    float y_scale = ((float)height)/((float)map_dim.y);
    *sx = x * x_scale;
    *sy = y * y_scale;
    *sx += screen_x_offset;
    *sy += screen_y_offset;
}

void draw_spawners(float z)
{
    // read all spawners
    // translate coordinates to map screen coordinate
    float sx,sy;
    const int w = 8;
    const int h = 8;
    for (int team=1; team<(int)N_TEAMS+1; team++)
    {
        if (ClientState::playerAgent_state.you != NULL
            && ClientState::playerAgent_state.you->status.team != 0
            && ClientState::playerAgent_state.you->status.team != team)
            continue;
        glBindTexture(GL_TEXTURE_2D, spawner_icons[team-1]);
        for (int i=0; i<ClientState::spawner_list->n_max; i++)
        {
            Spawner* s = ClientState::spawner_list->a[i];
            if (s == NULL) continue;
            if (s->team != team) continue;
            world_to_map_screen_coordinates(s->x, s->y, &sx, &sy);
            draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
        }
    }
}

void draw_bases(float z)
{
    float sx,sy;
    const int w = 8;
    const int h = 8;

    int team = 0;
    if (ClientState::playerAgent_state.you != NULL
        && ClientState::playerAgent_state.you != NULL)
        team = ClientState::playerAgent_state.you->status.team;

    Base* b = NULL;
    if (team == 0)
    {   // draw both
        glBindTexture(GL_TEXTURE_2D, base_icons[0]);
        b = ClientState::ctf->one.base;
        world_to_map_screen_coordinates(b->x, b->y, &sx, &sy);
        draw_bound_texture(sx - w/2, sy - h/2, w, h, z);

        glBindTexture(GL_TEXTURE_2D, base_icons[1]);
        b = ClientState::ctf->two.base;
        world_to_map_screen_coordinates(b->x, b->y, &sx, &sy);
        draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, base_icons[team-1]);
        if (team == 1)
            b = ClientState::ctf->one.base;
        else if (team == 2)
            b = ClientState::ctf->two.base;

        if (b != NULL)
        {
            world_to_map_screen_coordinates(b->x, b->y, &sx, &sy);
            draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
        }
    }
}

void draw_flags(float z)
{
    float sx,sy;
    const int w = 8;
    const int h = 8;

    int team = 0;
    if (ClientState::playerAgent_state.you != NULL
        && ClientState::playerAgent_state.you != NULL)
        team = ClientState::playerAgent_state.you->status.team;

    Flag* f = NULL;
    if (team == 0)
    {   // draw both
        glBindTexture(GL_TEXTURE_2D, flag_icons[0]);
        f = ClientState::ctf->one.flag;
        world_to_map_screen_coordinates(f->x, f->y, &sx, &sy);
        draw_bound_texture(sx - w/2, sy - h/2, w, h, z);

        glBindTexture(GL_TEXTURE_2D, flag_icons[1]);
        f = ClientState::ctf->two.flag;
        world_to_map_screen_coordinates(f->x, f->y, &sx, &sy);
        draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, flag_icons[team-1]);
        if (team == 1)
            f = ClientState::ctf->one.flag;
        else if (team == 2)
            f = ClientState::ctf->two.flag;

        if (f != NULL)
        {
            world_to_map_screen_coordinates(f->x, f->y, &sx, &sy);
            draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
        }
    }
}

void draw_items(float z)
{
    draw_bases(z);
    draw_flags(z);
    draw_spawners(z);
}


void draw()
{   //  double buffered texture swap indices
    static int draw_map_texture_index = 0;
    static int update_map_texture_index = 1;
    static int draw_overlay_texture_index = 0;
    static int update_overlay_texture_index = 1;
    
    static unsigned int update_counter = 0;

    if(update_counter % 6 == 0)
    {
        update_terrain_map(update_map_texture_index);
        draw_map_texture_index++;
        draw_map_texture_index%=2;
        update_map_texture_index++;
        update_map_texture_index%=2;
    }

    if (update_counter % 2 == 0)
    {
        update_indicators(update_overlay_texture_index);
        draw_overlay_texture_index++;
        draw_overlay_texture_index%=2;
        update_overlay_texture_index++;
        update_overlay_texture_index%=2;
    }

    update_counter++;

    static const float z = -0.03f;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, map_textures[draw_map_texture_index]);
    draw_bound_texture(screen_x_offset, screen_y_offset, width, height, z);

    draw_items(z+0.01);

    glBindTexture(GL_TEXTURE_2D, overlay_textures[draw_overlay_texture_index]);
    draw_bound_texture(screen_x_offset, screen_y_offset, width, height, z+0.02);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void teardown()
{
    if (overlay_surface != NULL)
        SDL_FreeSurface(overlay_surface);

    if (map_surface != NULL)
        SDL_FreeSurface(map_surface);

    if (gradient_surface != NULL)
        SDL_FreeSurface(gradient_surface);

    if (cells != NULL)
        free(cells);
}

}
