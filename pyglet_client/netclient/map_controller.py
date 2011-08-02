
class MapControllerGlobal:
    mapController = None
    @classmethod
    def init_0(cls):
        MapControllerGlobal.mapController = MapController()
    @classmethod
    def init_1(cls):
        cls.mapController.init()

from game_state import GameStateGlobal
from net_out import NetOut

import cube_lib.terrain_map as terrain_map


#time.clock()
class MapController:
    #terrainMap = None
    mapMessage = None
    @classmethod
    def init(self):
        #self.terrainMap = GameStateGlobal.terrainMap
        assert NetOut.mapMessage != None
        self.mapMessage = NetOut.mapMessage
        assert self.mapMessage != None
    def __init__(self):
        pass

    def process_chunk_list(self, list):
        print "Processing Chunk List: %i chunks" %(len(list))
        for (x,y,z,version) in list:
            terrain_map.set_server_version(x,y,z,version)
            #print "Chunks: %i, %i, %i" % (x,y,z)

    def tick(self):
        pass

    def incoming_map_chunk(self, x,y,z):
        if self.requests.has_key((x,y,z)):
            del self.requests[(x,y,z)]
        else:
            print "MapController.incoming_map_chunk map chunk key does not exist, (%i, %i, %i)" % (x,y,z)
            assert False

    def send_request(self, x,y,z):
        print "map chunk request: " + str((x,y,z))
        self.mapMessage.request_chunk(x,y,z)

