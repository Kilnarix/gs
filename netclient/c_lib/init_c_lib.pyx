from libcpp cimport bool

"""
Init
[gameloop]
"""
cdef extern from "c_lib.hpp":
    int init_c_lib()
    void close_c_lib()

_init = 0
def init():
    global _init
    if _init == 0:
        init_c_lib()
    _init = 1

def close():
    close_c_lib()

"""
Timer
[gameloopm netout]
"""
cdef extern from "../c_lib/time/physics_timer.h":
    void _START_CLOCK()
    int _GET_TICK()
    int _GET_MS_TIME()

#DEPRECATE
def StartPhysicsTimer():
    _START_CLOCK()

#DEPRECATE
def PhysicsTimerTickCheck():
    return _GET_TICK()

#DEPRECATE
def get_time():
    return _GET_MS_TIME();

#DEPRECATE
def get_tick():
    return _GET_TICK()

def START_CLOCK():
    _START_CLOCK()

def GET_TICK():
    return _GET_TICK()

def GET_MS_TIME():
    return _GET_MS_TIME();

"""
Network
[gameloop, netclient]
"""
cdef extern from "./net_lib/host.hpp":
    void client_dispatch_network_events()
    void client_connect_to(int a, int b, int c, int d, unsigned short port)
    void flush_to_net()

def NetClientDispatchNetworkEvents():
    client_dispatch_network_events()

def NetClientConnectTo(int a, int b,int c, int d, unsigned short _port):
    client_connect_to(a, b, c, d, _port)

def NetClientFlushToNet():
    flush_to_net()

"""
Python specific network
[net_*]
"""
cdef extern from "./net_lib/export.hpp":
    ctypedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id)
    ctypedef void (*PY_CLIENT_EVENT_CALLBACK)(int client_id, int event_type)
    void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)
    void set_python_net_event_callback_function(PY_CLIENT_EVENT_CALLBACK pt)
    void send_python_net_message(char* message, int size, int client_id)
    int _get_client_id()
    int _check_connection_status()

_CLIENT_CREATION_CALLBACK = None
_CLIENT_DELETION_CALLBACK = None
_CLIENT_MESSAGE_CALLBACK = None

def register_client_creation(function):
    global _CLIENT_CREATION_CALLBACK
    _CLIENT_CREATION_CALLBACK = function

def register_client_deletion(function):
    global _CLIENT_DELETION_CALLBACK
    _CLIENT_DELETION_CALLBACK = function

def register_client_message_handling(function):
    global _CLIENT_MESSAGE_CALLBACK
    _CLIENT_MESSAGE_CALLBACK = function

def get_client_id():
    return _get_client_id()

def connected():
    return _check_connection_status()

cdef void py_net_message_callback(char* buff, int n, int client_id):
    ustring = buff[:n]
    #ustring1 = ustring2
    if(_CLIENT_MESSAGE_CALLBACK != None):
        _CLIENT_MESSAGE_CALLBACK(client_id, ustring)

def _send_python_net_message(message, int client_id):
    #print "Send python net message"
    cdef int length = len(message)
    cdef char* c_string = message
    send_python_net_message(message, length, client_id)

cdef void py_net_net_event_callback(int client_id, int event_type):
    if event_type == 0:
        print "Client connected: %i" % (client_id)
        _CLIENT_CREATION_CALLBACK(client_id)
    if event_type == 1:
        print "Client disconnected: %i" % (client_id)
        _CLIENT_DELETION_CALLBACK(client_id)
        
cpdef init_python_net():
    cdef PY_MESSAGE_CALLBACK p = py_net_message_callback
    set_python_net_callback_function(py_net_message_callback)
    print "Python net callback set"
    set_python_net_event_callback_function(py_net_net_event_callback)

"""
sound
[sound]
-- move the init logic into C
"""
cdef extern from "./sound/sound.hpp" namespace "Sound":
    void set_volume(float vol)
    void set_enabled(int y)
    void set_sound_path(char* path)

    void load_sound(char* file)
    void update_sound()
    

class Sound(object):

    @classmethod
    def init(cls, char* sound_path, sounds=[], enabled=True, float sfxvol=1.0, float musicvol=1.0):
        if not len(sounds):
            set_enabled(0)
        if sfxvol <= 0. and musicvol <= 0.:
            set_enabled(0)

        set_enabled(int(enabled))
        set_volume(sfxvol)
        set_sound_path(sound_path)

        for snd in sounds:
            load_sound(snd)

    @classmethod
    def update(cls):
        update_sound()

"""
SDL
[gameloop]
"""

cdef extern from "./SDL/SDL_functions.h":
    int _set_resolution(int xres, int yres, int fullscreen)
    int _init_video()
    void _del_video()
    int _swap_buffers()
    int _get_ticks()

def flip():
    _swap_buffers()

def get_ticks():
    return _get_ticks()

def set_resolution(xres, yres, fullscreen = 0):
    _set_resolution(xres, yres, fullscreen)

"""
Game modes (CTF)
[chat client (sends "join team" cmd)]
"""

cdef extern from "./game/teams.hpp":
    cdef cppclass CTFTeam:  # inherits Team
        pass

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        void join_team(int team)
        bool auto_assign

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    CTF ctf

def join_team(int team):
    ctf.join_team(team)

"""
Options & Settings
[options]
-- this is one of the few things to keep in cython until the end
"""

def load_options(opts):
    ctf.auto_assign = opts.auto_assign_team

"""
Camera
[gameloop]
"""
cdef extern from "./camera/camera.hpp":
    cdef cppclass CCamera:
        float fov
        float x_size
        float y_size
        float z_near
        float z_far
        float x
        float y
        float z
        float theta
        float phi
        float xl, yl, zl
        float xu, yu, zu
        float ratio

        void move(float dx, float dy, float dz)
        void set_aspect(float fov, float z_near, float z_far)
        void set_projection(float x, float y, float z, float theta, float phi)
        void set_dimensions()   # uses SDL_functions' xres & yres properties
        void set_fov(float fov)
        
    cdef enum CAMERA_TYPES:
        UNKNOWN_CAM
        AGENT_CAM
        CAMERA_CAM

    CCamera* get_agent_camera()
    CCamera* get_free_camera()
    void use_agent_camera()
    void use_free_camera()
    void update_agent_camera()
    void camera_input_update(int delta_tick, bool invert, float sensitivity)
    void world_projection()
    void hud_projection()

camera_properties = [
    'fov',
    'x_size', 'y_size',
    'z_near', 'z_far',
    'x', 'y', 'z',
    'theta', 'phi',
    'xl', 'yl', 'zl',
    'xu', 'yu', 'zu',
    'ratio',
]

camera_callback = None

cdef class Camera(object):
    cdef CCamera* camera

    def __init__(self, name="free"):
        cdef CCamera* cam

        if name == 'free':
            cam = get_free_camera()
        elif name == 'agent':
            cam = get_agent_camera()
        else:
            print "Unknown camera name %s." % (name,)
            raise ValueError
            
        if cam == NULL:
            print "Cython camera init -- Camera is null"
            raise ValueError

        self.camera = cam

        self.camera.set_dimensions()

    def set_fov(self, float fov):
        self.camera.set_fov(fov)
            
    def set_aspect(self, float fov, float z_near, float z_far):
        self.camera.set_aspect(fov, z_near, z_far)

    def set_projection(self, float x, float y, float z, float theta, float phi):
        self.camera.set_projection(x,y,z, theta, phi)

    def move(self, float dx, float dy, float dz):
        self.camera.move(dx, dy, dz)

    def forward(self):
        return [self.camera.xl, self.camera.yl, self.camera.zl]
        
    def normal(self):
        return [self.camera.xu, self.camera.yu, self.camera.zu]
        
    def __getattribute__(self, name):
        if name == 'x':
            return self.camera.x
        elif name == 'y':
            return self.camera.y
        elif name == 'z':
            return self.camera.z
        elif name == 'xl':
            return self.camera.xl
        elif name == 'yl':
            return self.camera.yl
        elif name == 'zl':
            return self.camera.zl
        elif name == 'xu':
            return self.camera.xu
        elif name == 'yu':
            return self.camera.yu
        elif name == 'zu':
            return self.camera.zu
            
        elif name == 'theta':
            return self.camera.theta
        elif name == 'phi':
            return self.camera.phi

        elif name == 'ratio':
            return self.camera.ratio
        elif name == 'fov':
            return self.camera.fov
        elif name == 'x_size':
            return self.camera.x_size
        elif name == 'y_size':
            return self.camera.y_size

        elif name == 'z_near':
            return self.camera.z_near
        elif name == 'z_far':
            return self.camera.z_far

        else:
            return object.__getattribute__(self, name)

    def __setattr__(self, k, v):
        if k == 'x':
            self.camera.x = v
        elif k == 'y':
            self.camera.y = v
        elif k == 'z':
            self.camera.z = v
        elif k == 'xl':
            self.camera.xl = v
        elif k == 'yl':
            self.camera.yl = v
        elif k == 'zl':
            self.camera.zl = v
        elif k == 'xu':
            self.camera.xu = v
        elif k == 'yu':
            self.camera.yu = v
        elif k == 'zu':
            self.camera.zu = v
        elif k == 'theta':
            self.camera.theta = v
        elif k == 'phi':
            self.camera.phi = v

        elif k == 'ratio':
            self.camera.ratio = v
        elif k == 'fov':
            self.camera.fov = v
        elif k == 'x_size':
            self.camera.x_size = v
        elif k == 'y_size':
            self.camera.y_size = v

        elif k == 'z_near':
            self.camera.z_near = v
        elif k == 'z_far':
            self.camera.z_far = v

        else:
            self.__dict__[k] = v

class CyCamera(object):
    @classmethod
    def use_agent_camera(cls):
        use_agent_camera()
    @classmethod
    def use_free_camera(cls):
        use_free_camera()
    @classmethod
    def camera_input_update(cls, int delta_tick, bool invert, float sensitivity):
        camera_input_update(delta_tick, invert, sensitivity)
    @classmethod
    def world_projection(cls):
        world_projection()
    @classmethod
    def hud_projection(cls):
        hud_projection()
    @classmethod
    def update_agent_camera(cls):
        update_agent_camera()
    
"""
Animations
[gameloop]
"""
cdef extern from "./animations/animations.hpp" namespace "Animations":
    void animations_tick()
    void animations_draw()

def AnimationTick():
    animations_tick()

def AnimationDraw():
    animations_draw()

"""
Agents
[chat, gameloop, netclient, netout, hud]
"""

cdef extern from "./agent/agent_status.hpp":
    unsigned int PLAYER_NAME_MAX_LENGTH
    cdef cppclass Agent_status:
        bool dead
        int team
        char* name

#collision box
cdef extern from "./agent/agent.hpp":
    cdef struct Agent_collision_box:
        float b_height
        float c_height
        float box_r

#AgentState
cdef extern from "./agent/agent.hpp":
    cdef cppclass AgentState:
        int seq
        float theta
        float phi
        float x,y,z
        float vx,vy,vz
        float camera_height
 
    cdef cppclass Agent_state:
        int id
        AgentState s
        Agent_collision_box box
        Agent_status status

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        Agent_state* get(int id)
        Agent_state* get_or_create(int id)
        int get_ids()
        int* ids_in_use

cdef extern from "./agent/player_agent.hpp":
    cdef cppclass PlayerAgent_state:
        int agent_id
        float camera_height()
        void update_sound()
        void display_agent_names()


cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Agent_list agent_list
    PlayerAgent_state playerAgent_state
'''
WRAPPER
'''
class AgentWrapper(object):
    properties = [
        'x', 'y', 'z',
        'vx', 'vy', 'vz',
        'theta', 'phi',
        'crouch_height','c_height',
        'box_height', 'b_height',
        'box_r',
        'dead',
        'team',
        'name',
    ]

    def __init__(self, int id):
        self.id = id
        
    def __getattribute__(self, name):
        if name not in AgentWrapper.properties:
            raise AttributeError

        cdef Agent_state* a
        cdef int i
        i = object.__getattribute__(self,'id')
        a = agent_list.get(i)

        if a == NULL:
            print "AgentWrapper.__getattribute__ :: agent %d not found" % (i,)
            raise ValueError, "C Agent %d not found" % (i,)

        if name == 'x':
            return a.s.x
        elif name == 'y':
            return a.s.y
        elif name == 'z':
            return a.s.z
        elif name == 'vx':
            return a.s.vx
        elif name == 'vy':
            return a.s.vy
        elif name == 'vz':
            return a.s.vz
        elif name == 'theta':
            return a.s.theta
        elif name == 'phi':
            return a.s.phi

        elif name == 'crouch_height' or name == 'c_height':
            return a.box.c_height
        elif name == 'box_height' or name == 'b_height':
            return a.box.b_height
        elif name == 'box_r':
            return a.box.box_r

        elif name == 'dead':
            return a.status.dead

        elif name == 'team':
            return a.status.team

        elif name == 'name':
            return a.status.name

        print 'AgentWrapper :: Couldnt find %s. There is a problem' % name
        raise AttributeError

class PlayerAgentWrapper(object):

    properties = ['camera_height',]

    def __init__(self, int id):
        self.id = id

    def __getattribute__(self, name):
        if name not in PlayerAgentWrapper.properties:
            raise AttributeError

        if name == 'camera_height':
            return playerAgent_state.camera_height()

        print "PlayerAgentWrapper :: couldnt find %s. There is a problem" % name
        raise AttributeError

    def update_sound(self):
        playerAgent_state.update_sound()

    def display_agent_names(self):
        playerAgent_state.display_agent_names()

def get_player_agent_id():
    return playerAgent_state.agent_id

class AgentListWrapper:

    @classmethod
    def add(cls, int id):
        agent_list.get_or_create(id)
        return id

    @classmethod
    def ids(cls):
        cdef int n
        n = agent_list.get_ids()
        ids = []
        for k in range(n):
            ids.append(agent_list.ids_in_use[k])
        return ids

""" Chat """

cdef extern from "./input/handlers.hpp":
    int CHAT_BUFFER_SIZE
    int* chat_input_buffer_unicode
    char** chat_input_buffer_sym
    int chat_cursor_index
    void clear_chat_buffer()

def get_chat_input_buffer():
    sym_buff = []
    uni_buff = []
    for i in range(chat_cursor_index):
        sym = chat_input_buffer_sym[i]
        try:
            uni = unichr(chat_input_buffer_unicode[i])
        except:
            uni = sym
        sym_buff.append(sym)
        uni_buff.append(uni)
    return sym_buff, uni_buff

def clear_chat_input_buffer():
    clear_chat_buffer()
    
""" Input """

cdef extern from "./input/input.hpp":
    int _get_key_state()
    int _process_events()
    
def get_key_state():
    _get_key_state()
def process_events():
    _process_events()

cdef extern from "./input/handlers.hpp":

    cdef enum InputStateMode:
        INPUT_STATE_AGENT
        INPUT_STATE_CAMERA

    cdef struct InputState:
        bool mouse_bound
        bool help_menu
        bool inventory
        bool scoreboard
        bool map
        bool chat
        bool hud

        bool can_jump
        bool quit

        InputStateMode input_mode
        InputStateMode camera_mode

    InputState input_state

class CyInputState(object):
    def __setattr__(self,k,v):
        if k == 'mouse_bound':
            input_state.mouse_bound = v
        elif k == 'help_menu':
            input_state.help_menu = v
        elif k == 'inventory':
            input_state.inventory = v
        elif k == 'scoreboard':
            input_state.scoreboard = v
        elif k == 'map':
            input_state.map = v
        elif k == 'chat':
            input_state.chat = v
        elif k == 'hud':
            input_state.hud = v
        elif k == 'can_jump':
            input_state.can_jump = v
        elif k == 'quit':
            input_state.quit = v
        elif k == 'input_mode':
            input_state.input_mode = v
        elif k == 'camera_mode':
            input_state.camera_mode = v
        else:
            raise AttributeError
    def __getattribute__(self, k):
        if k == 'mouse_bound':
            return input_state.mouse_bound
        elif k == 'help_menu':
            return input_state.help_menu
        elif k == 'inventory':
            return input_state.inventory
        elif k == 'scoreboard':
            return input_state.scoreboard
        elif k == 'map':
            return input_state.map
        elif k == 'chat':
            return input_state.chat
        elif k == 'hud':
            return input_state.hud
        elif k == 'can_jump':
            return input_state.can_jump
        elif k == 'quit':
            return input_state.quit
        elif k == 'input_mode':
            return input_state.input_mode
        elif k == 'camera_mode':
            return input_state.camera_mode
        else:
            raise AttributeError    
    
cy_input_state = CyInputState()

"""
HUD
-- this is here because hud.py needs to tell it to render certain things
"""

cdef extern from "./hud/hud.hpp" namespace "Hud":
    void set_hud_draw_settings(
        bool disconnected,
        bool dead,
        bool fps,
        float fps_val,
        bool ping,
        int ping_val,
    )
    void draw_hud()
    void set_chat_message(int i, char* text, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    void set_chat_input_string(char* text)
    void update_hud_draw_settings()

cdef class HUD:
    @classmethod
    def draw(cls):
        draw_hud()
    @classmethod
    def set_draw_settings(cls,
        bool disconnected,
        bool dead,
        bool fps,
        float fps_val,
        bool ping,
        int ping_val,
    ):
        set_hud_draw_settings(
            disconnected,
            dead,
            fps,
            fps_val,
            ping,
            ping_val,
        )
    @classmethod
    def set_chat_message(cls, i, text, color):
        cdef unsigned char r
        cdef unsigned char g
        cdef unsigned char b
        cdef unsigned char a
        r,g,b,a = color
        set_chat_message(i, text, r,g,b,a)
    @classmethod
    def set_chat_input_string(cls, text):
        set_chat_input_string(text)
    @classmethod
    def update_hud_draw_settings(cls):
        update_hud_draw_settings()

"""
Font
[hud]
-- The hud depdnency is only the calling of the init() method
-- No python is dependent on this code here; its mostly doing parsing
-- It does need to add font set metadata, however
"""
cdef extern from "./hud/font.hpp" namespace "HudFont":
    int load_font(char* filename)

    void add_glyph(
        int c,
        float x, float y,
        float xoff, float yoff,
        float w, float h,
        float xadvance
    )
    void set_missing_character(int cc)

import os.path

class Font:

    font_path = "./media/fonts/"
    font_ext = ".fnt"
    png_ext = "_0.png"
    missing_character = '?'
    glyphs = {}
    info = {}
    font = None

    ready = False
    
    @classmethod
    def init(cls):
        if not os.path.exists(cls.font_path):
            print "ERROR c_lib_fonts.pyx :: cannot find font path %s" % (cls.font_path,)
            cls.ready = False
            return

        import opts
        cls.font = cls(opts.opts.font)
        cls.font.parse()
        cls.font.load()

    def __init__(self, fn):
        self.fontfile = fn
        self.pngfile = ''
        self.process_font_filename()
#        self._gen_stress()
        
    def process_font_filename(self):
        fn = self.fontfile
        fn = fn.split('.')[0]
        fn += self.font_ext
        fn = self.font_path + fn
        self.fontfile = fn
        if not os.path.exists(self.fontfile):
            print "ERROR c_lib_fonts.pyx :: cannot find font file %s in %s" % (fn, self.font_path,)
            self.ready = False
            return
        self.ready = True
            
    def parse(self):
        png = ""
        
        # parse .fnt
        with open(self.fontfile) as f:
            lines = f.readlines()
            for line in lines:
                line = line.strip()
                tags = line.split()
                name = tags[0]
                tags = dict(map(lambda a: a.split('='), tags[1:]))

                if name == 'page':
                    png = tags['file'].strip('"')
                elif name == 'info':
                    self.info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'common':
                    self.info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'chars':
                    print '%s characters in font set' % (tags['count'],)
                elif name == 'char':
                    self.glyphs[int(tags['id'])] = tags

        # process png filename
        if not png:
            png = self.fontfile + self.png_ext
        fp_png = self.font_path + png
        if not os.path.exists(fp_png):
            print "ERROR c_lib_fonts.pyx :: cannot find font png file %s in %s" % (fp_png, self.font_path,)
            self.ready = False
            return
        self.pngfile = fp_png

        self.clean_glyphs()
        self.missing_character_available()

    def add_glyphs_to_c(self):
        for char_code, glyph in self.glyphs.items():
            x = float(glyph['x'])
            y = float(glyph['y'])
            xoff = float(glyph['xoffset'])
            yoff = float(glyph['yoffset'])
            w = float(glyph['width'])
            h = float(glyph['height'])
            xadvance = float(glyph['xadvance'])
            add_glyph(char_code, x, y, xoff, yoff, w,h, xadvance)

            if char_code == ord(' '):
                add_glyph(ord('\t'), x,y, xoff, yoff, w,h, xadvance)
                
    def clean_glyphs(self):
        for kc, glyph in self.glyphs.items():
            for k,v in glyph.items():
                try:
                    glyph[k] = int(glyph[k])
                except ValueError:
                    pass

    def missing_character_available(self):
        cc = ord(self.missing_character)
        if cc not in self.glyphs:
            print "ERROR Missing character placeholder %s is not a known glyph" % (self.missing_character,)
            self.ready = False
            return False
        set_missing_character(cc)
        return True
        
    def load(self):
        if not load_font(self.pngfile):
            self.ready = False
            return
        self.add_glyphs_to_c()
        self.ready = True


"""
Client State
[gameloop]
"""

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    void update_client_state()
    void draw_client_state()
    void tick_client_state()

    void send_identify_packet(char* name)

class ClientState(object):
    @classmethod
    def update(cls):
        update_client_state()
    @classmethod
    def draw(cls):
        draw_client_state()
    @classmethod
    def tick(cls):
        tick_client_state()

def identify(name):
    send_identify_packet(name)


