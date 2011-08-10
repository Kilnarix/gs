'''
Projectiles
'''

'''
Projectile data (will be moved to configurable external format)
'''

from cube_lib.terrain_map import collisionDetection

projectile_dat = {

    1 : {
        'speed' : 100,
        'damage' : 20,
        'ttl_max' : 400, #time to live in ticks
        'penetrates': False,
        'suicidal'  : False, # True for grenades
    #    'splash' : {
    #    'radius' : 3,
    #    'damage' : 15,
    #    'force' : 150,
    },

    2 : {
        'speed' : 100,
        'damage' : 20,
        'ttl_max' : 400
    },

}


'''
Projectile Controller
'''
from object_lists import GenericObjectList
from game_state import GameStateGlobal

class ProjectileList(GenericObjectList):

    def __init__(self):
        from projectiles import Projectile
        GenericObjectList.__init__(self)
        self._metaname = 'ProjectileList'
        self._itemname = 'Projectile'
        self._object_type = Projectile

    def create(self, *args, **projectile):
        projectile = self._add(*args, **projectile)
        return projectile

    def destroy(self, projectile):
        self._remove(projectile)
        return projectile


'''
Projectile class
'''
from math import sin, cos, pi
#from game_objects import GameObject
from utils import filter_props
#from cube_dat import CubeGlobal

class Projectile:

    def __init__(self, state=None, type=None, owner=None): #more args
        if None in (state, type,):
            print 'Projectile __init__ missing args'
            raise TypeError

        global projectile_dat
        assert projectile_dat.has_key(type)
        p = projectile_dat[type]
        #load projectile settings

        self.state = map(float, state)
        x, y, z, vx, vy, vz = state

        self.id = GameStateGlobal.new_projectile_id()
        self.type = type
        self.speed = p['speed']
        self.damage = p['damage']
        self.ttl = 0
        self.ttl_max = p['ttl_max']
        self.penetrates = p['penetrates']
        self.suicidal = p['suicidal']

        self.owner = owner

    def update(self, **args):
        try:
            state = args['state']
            state = list(state)
            assert len(state) == 6
            self.state = state
        except KeyError:
            pass
        except TypeError:
            print 'projectile update :: state is not iterable'
            pass
        except AssertionError:
            print 'projectile update :: state is wrong length'
            return

    #run this once per frame for each projectile
    def tick(self):
        x,y,z,vx,vy,vz = self.state

        fps = 30. # frame per second
        speed = self.speed / fps

        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.delete()
            return

        x += vx / fps
        y += vy / fps
        z += vz / fps

        if collisionDetection(int(x), int(y), int(z)):
            print "collision with wall"
            self.delete()
            return

        #slow way, will be bottle neck later
#        for agent in GameStateGlobal.agentList.values():
#            if agent.point_collision_test(x,y,z):
#                print "projectile collision"
#                agent.take_damage(self.damage)

        #faster way; needs to choose a large radius and only update every n-frames
        agent_list = GameStateGlobal.agentList.agents_near_point(x, y, z, 4.0)
        for agent in agent_list:
            if agent.point_collision_test(x,y,z):
                if not self.suicidal and agent.owner == self.owner: # bullet is hitting yourself, and bullet doesnt kill yourself
                    continue
                print "projectile collision"
                agent.take_damage(self.damage, self.owner)
                if not self.penetrates:
                    self.delete()
                    return
        #agent_hit = GameStateGlobal.agentList.at((x, y, z,))
        #if agent_hit != False:
        #    agent_hit.take_damage(self.damage)

        self.state = [x,y,z,vx,vy,vz]

    def delete(self):
        GameStateGlobal.projectileList.destroy(self)

    def json(self, properties=None): # json encodable string representation
        d = {
            'id'    :   self.id,
            'type'  :   self.type,
        }
        if properties is None:
            d.update({
                'state' : self.state,
                'owner' : self.owner,
            })
        else:
            d.update(filter_props(self, properties))
        return d
