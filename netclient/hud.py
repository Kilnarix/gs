import opts
opts = opts.opts

import init_c_lib

from chat_client import ChatClientGlobal
from input import InputGlobal
from net_client import NetClientGlobal
from game_state import GameStateGlobal

'''
HUD overlay
'''

base_dir = "./media/"

class Hud(object):

    def __init__(self):

        init_c_lib.Font.init()

        self.win_width = opts.width
        self.win_height = opts.height
        self.font_height = opts.font_size
        self.height_margin = 5
        self.width_margin = 3

        self._init_reticles()
        self._init_text_dict()
        self._init_scoreboard()
        self._init_player_stats()
        self._init_help_menu()

        self.inventory = init_c_lib.Inventory(opts.inventory_hud_x_offset, opts.inventory_hud_y_offset)
        self.cube_selector = init_c_lib.CubeSelector(opts.cube_selector_x_offset, opts.cube_selector_y_offset)

        self.disconnected_message = self.text(
            text = 'Server not connected',
            x = self.win_width/2 - 80,
            offset = self.win_height/2 + self.font_height*2,
            color = (255,10,10,255),
        )
        
        self.dead_message = self.text(
            text = 'You died',
            x = self.win_width/2 - 80,
            offset = self.win_height/2 + self.font_height*2,
            color = (255,10,10,255),
        )

        self.fps = self.text(
            text = '',
            x = 0 + self.width_margin,
            offset = self.win_height - self.font_height - self.height_margin
        )

        self.ping = self.text(
            text = '',
            x = 0 + self.width_margin,
            offset = self.win_height - (self.font_height * 2) - self.height_margin
        )

    def _init_reticles(self):
        tex_file = '%stexture/target.png' % (base_dir,)
        self.reticle = init_c_lib.Reticle(tex_file, self.win_width, self.win_height)
        tex_file = '%stexture/target-zoom.png' % (base_dir,)
        self.scope_reticle = init_c_lib.Reticle(tex_file, self.win_width, self.win_height)

    def _init_text_dict(self):
        offset = 50
        msg_height = 0
        line_height = 20
        msg_count = range(ChatClientGlobal.chatRender.MESSAGE_RENDER_COUNT_MAX)

        blanks = [self.text(text='', x=50, offset=(offset + (line_height * i) + msg_height)) for i in msg_count]
        self.text_dict = dict(zip(msg_count, blanks))
        self.text_dict['input'] = self.text(text='', offset=200, x=50)

    def _init_player_stats(self):
        self.player_stats = self.text(
            text = '',
            offset = self.win_height - self.font_height - self.height_margin,
            x = self.win_width - 360
        )

    def _init_help_menu(self):
        self.help_menu = self.text(
            text = self.help_menu_text,
            offset = 0,
            x = self.win_width/2
        )

    def scoreboard_stats(self):
        props = ['name', 'kills', 'deaths', 'score', 'id']
        defs =  [[] for i in range(len(props))]
        stats = dict(zip(props, defs))
        stats['team'] = {}

        agents = GameStateGlobal.agentList.values()

        agents.sort(key=lambda a: a.team.id)

        curr_team = 0
        for i, agent in enumerate(agents):
            if agent.team.id and agent.team.id != curr_team:
                curr_team = agent.team.id
                stats['team'][i] = agent.team
            stats['name'].append(agent.name)
            stats['kills'].append(agent.kills)
            stats['deaths'].append(agent.deaths)
            stats['score'].append(agent.score)
            stats['id'].append(agent.id)

        return stats

    def _init_scoreboard(self):
        self._scoreboard_properties = ['ID', 'Name', 'Kills', 'Deaths', 'Score']
        self.scoreboard = {}
        col_width = (self.win_width * 0.75) // len(self._scoreboard_properties)
        start_x = self.win_width // 8
        i = 0
        for col_name in self._scoreboard_properties:
            self.scoreboard[col_name.lower()] = self.text(
                text = '',
                x = start_x + (i * col_width),
                color = (150, 150, 255, 255)
            )
            i += 1

        self.team_names = {
            1 : self.text(
                text = '',
                x = start_x,
                offset = (self.win_height // 8),
                color = (150, 150, 255, 255)
            ),
            2 : self.text(
                text = '',
                x = start_x,
                offset = (self.win_height // 8),
                color = (150, 150, 255, 255)
            )
        }

    def _format_scoreboard_plain(self, stats):
        for prop in self._scoreboard_properties:
            lprop = prop.lower()
            lines = []
            lines.append(prop + '\n')
            vals = stats[lprop]
            for i, val in enumerate(vals):
                if i in stats['team']:
                    lines.append('\n')
                lines.append(str(val))
            stats[lprop] = '\n'.join(lines)

        return stats

    def draw_fps(self, fps_text):
        self.fps.text = str(fps_text)
        self.fps.draw()

    def draw_ping(self, ping_text):
        self.ping.text = '%sms' % (str(ping_text),)
        self.ping.draw()

    def _format_player_stats_plain(self):
        agent = GameStateGlobal.agent
        if agent is None:
            s = 'No agent yet.'
        else:
            if agent.team is None:
                s = 'No team yet.'
            else:
                if agent.team.viewers:
                    s = 'Viewer Mode'
                else:
                    w = 'Ammo %s' % (agent.weapon_hud_display(),)
                    hp = 'HP %i' % (agent.health,)
                    coins = '$%d' % (agent.coins,)
                    s = '%s :: %s :: %s' % (coins, hp, w,)
        return s

    def draw_player_stats(self):
        stats = self._format_player_stats_plain()
        old = self.player_stats.text
        if old != stats:
            self.player_stats.text = stats
        self.player_stats.draw()

    def draw_reticle(self):
        self.reticle.draw()
        #self.draw_reference_center()

    def draw_scope_reticle(self):
        self.scope_reticle.draw()

    def draw_reference_center(self):
        w = 2
        x = (self.win_width/2) - w/2
        y = (self.win_height/2) - w/2
        self._draw_square(x, y, w, color=(255,10,10))

    def draw_scoreboard(self):
        stats_txt = self._format_scoreboard_plain(self.scoreboard_stats())
        for key, txt in stats_txt.items():
            if key == 'team': continue
            curr_sb = self.scoreboard[key]
            curr_sb.text = txt
            curr_sb.draw()

        # draw team names
        line_height = 18.
        y_offset = line_height * 3
        for i, (index, team) in enumerate(stats_txt['team'].items()):
            y_offset += line_height * (index + 1 + i)
            team_txt = team.name
            team_txt += ' ' * 4
            team_txt += str(team.score)
            self.team_names[team.id].y = self.team_names[team.id].offset - y_offset
            self.team_names[team.id].text = team_txt
            self.team_names[team.id].color = list(team.color) + [255]
            self.team_names[team.id].draw()

    def text(self, text='', offset=120, x=20, color=(255,40,0,255)):
        txt = init_c_lib.Text(
            text = text,
            x = x,
            y = self.win_height - offset,
            color = color
        )
        return txt

    def draw_chat_input(self):
        text = self.text_dict['input']
        text.text = ChatClientGlobal.chatRender.user_input()
        text.draw()

    def draw_chat_messages(self):
        to_draw = []
        offset = 20
        msg_height = 0
        line_height = 20
        i = 0
        msgs = ChatClientGlobal.chatRender.messages()
        pm_channel = 'pm_' + str(NetClientGlobal.connection.client_id)
        for msg in msgs:
            if not msg.payload.content.strip(): continue
            content = msg.name
            if content:
                content += ': '
            content += msg.payload.content
            channel = msg.payload.channel
            if channel == 'system':
                color = (40, 255, 0, 255)
            elif channel == pm_channel:
                color = (120, 200, 200, 255)
            else:
                color = (255, 40, 0, 255)
            txt = self.text_dict[i]
            if txt.text != content:
                txt.text = content
            txt.color = color
            to_draw.append(txt)
            i += 1

        for t in to_draw:
            t.draw()

    def draw_cursor(self):
        buff = ChatClientGlobal.chatRender.input_buffer()
        x = self.text_dict['input'].x
        y = self.text_dict['input'].y
        init_c_lib.draw_chat_cursor(''.join(buff), x, y)

    def draw_life_status(self):
        if GameStateGlobal.agent is not None and GameStateGlobal.agent.dead:
            self.dead_message.draw()

    def draw(self, fps=None, ping=None, cube_selector=False, zoom=False):
        if InputGlobal.vn:
            init_c_lib.VN.draw()
            return
        # draw non-text first
        if zoom:
            self.draw_scope_reticle()
            return
        self.draw_reticle()
        if cube_selector:
            self.cube_selector.draw()
        if InputGlobal.inventory:
            self.inventory.draw()

        if InputGlobal.input == 'chat':
            self.draw_cursor()

        if InputGlobal.map:
            init_c_lib.Map.draw()

        active_equipment_slot = -1
        if GameStateGlobal.agent:
            active_equipment_slot = GameStateGlobal.agent.hud_equipment_slot()
        init_c_lib.Equipment.draw(active_equipment_slot)

        init_c_lib.Compass.draw()

        # draw text
        init_c_lib.Font.font.start()
        self.draw_text_items(fps, ping)
        init_c_lib.Font.font.end()

    def draw_text_items(self, fps, ping):
        init_c_lib.draw_hud_billboard_text()
        
        if InputGlobal.help_menu:
            self.draw_help_menu()
            return

        self.draw_player_stats()

        if InputGlobal.scoreboard:
            self.draw_scoreboard()

        if fps is not None:
            self.draw_fps(fps)

        if ping is not None:
            self.draw_ping(ping)
            
        self.draw_chat_messages()
        if InputGlobal.input == 'chat':
            self.draw_chat_input()
        if not NetClientGlobal.connection.connected:
            self.disconnected_message.draw()
        else:
            self.draw_life_status()
        
    help_menu_text = """
    Key:            Action:

    Esc             Quit
    WASD            Move
    Space           Jump
    Z               Jetpack (hold down)
    
    G               Toggle camera
    T               Toggle keyboard

    R               Reload
    Num keys        Select weapon
    Mouse scroll    Select weapon
    Left click      Activate weapon
    Right click     Zoom (if weapon has scope)
    Arrow keys      Choose block type when block selector is active

    Y               Chat
    H               Display this menu
    Tab             Display scoreboard
    M               Minimap
    
    Weapons:
    1               Laser
    2               Pick
    3               Block selector / applier
    4               Grenades
    """
    def draw_help_menu(self):
        self.help_menu.draw()
