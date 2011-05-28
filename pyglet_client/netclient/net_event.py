#!/usr/bin/python

'''
Client network incoming
'''

import simplejson as json
#import struct

class NetEventGlobal:
    messageHandler = None
    mapMessageHandler = None
    @classmethod
    def init_0(self):
        self.messageHandler = MessageHandler()  ##MAY CAUSE ERRORS?
        self.mapMessageHandler = MapMessageHandler()
    @classmethod
    def init_1(self):
        pass
        MessageHandler.init()
        MapMessageHandler.init()

class MessageHandler:
    player = None #move this somewhere else
    @classmethod
    def init(self):
        self.player = GameStateGlobal.player
        self.agent = GameStateGlobal.agent
        assert self.player != None
        assert self.agent != None
    def __init__(self):
        pass

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1:       #json message
            self.process_json_event(msg_type, datagram)
        else:                   #create a process json message
            self.process_binary_event(msg_type, datagram)

#binary events
    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            NetEventGlobal.mapMessageHandler._map_chunk(datagram)
        elif msg_type == 4:
            self._4_
        else:
            print "MessageHandler.process_binary_event: message type unknown, " + str(msg_type)
#message events

    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
        except:
            print "MessageHandler.process_json_event error"
            print str(msg)
            return
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        if cmd == 'agent_position':
            self._agent_position(**msg)

        # initial settings
        elif cmd == 'client_id':
            if self._set_client_id(**msg):
                NetOut.sendMessage.identify()
            else:
                NetOut.sendMessage.request_client_id()
        elif cmd == 'identify_fail':
            msg = msg.get('msg', '')
            msg = 'Identification failed. %s' % (msg,)
            print msg
            # send system notification
            ChatClientGlobal.chatClient.system_notify('/identify_fail '+msg)
            ChatClientGlobal.chatClient.system_notify('/identify_fail Use /nick to set name.')
            # activate chat, insert /nick
            InputGlobal.enable_chat()
            ChatClientGlobal.chatClient.insert_string('/nick ')

        elif cmd == 'identified':
            self._on_identify(**msg)
            
        #map events
        elif cmd == 'chunk_list':
            NetEventGlobal.mapMessageHandler._chunk_list(**msg)
            #print "Chunk List Received"
            #print str(msg['list'])
        elif cmd == 'set_map':
            NetEventGlobal.mapMessageHandler._set_map(**msg)
        elif cmd == 'chat':
            ChatClientGlobal.chatClient.receive(msg)
        else:
            print "JSON message type unregonized"

    def _agent_position(self, id, tick, state, **misc):
        [x,y,z, vx,vy,vz, ax,ay,az] = state
        [x,y,z] = map(lambda k: float(k), [x,y,z])

        self.agent.x = x
        self.agent.y = y
        self.agent.z = z
        self.agent.vx = vx
        self.agent.vy = vy
        self.agent.vz = vz
        self.agent.ax = ax
        self.agent.ay = ay
        self.agent.az = az

    def _set_client_id(self, **msg):
        id = msg.get('id', None)
        if id is None:
            print '_register msg missing id'
            return False
        print "Received Client Id: %s" % (id,)
        NetClientGlobal.client_id = id
        return True

    def _on_identify(self, **msg):
        player = msg.get('player', None)
        if player is None:
            print 'msg::identified - missing player'
            return False

        name = player.get('name', None)
        if name is None:
            print 'msg::identified - player missing name'
            return False
        if type(name) != str:
            print 'msg::identified - name is not str'
            return False

        client_id = player.get('cid', None) # client_id is currently optional for server to send
        if client_id is not None:
            NetClientGlobal.client_id = client_id

        NetClientGlobal.username = name
        print 'Identified: name is %s' % (name,)
        ChatClientGlobal.on_identify()

        GameStateGlobal.update_info(player)
        return True

class MapMessageHandler:
    terrainMap = None
    mapChunkManager = None
    mapController = None
    @classmethod
    def init(self):
        self.terrainMap = GameStateGlobal.terrainMap
        self.mapChunkManager = MapChunkManagerGlobal.mapChunkManager
        self.mapController = MapControllerGlobal.mapController
        assert self.mapController != None
    def __init__(self):
        pass

    def _chunk_list(self, list, **msg):
        #print str(list)
        self.mapController.process_chunk_list(list)
        #for chunk in list:
        #    (x,y,z,version ) = chunk

    def _map_chunk(self, datagram):
        #print "Map Chunk Received"
        (x,y,z) = self.terrainMap.set_packed_chunk(datagram)
        self.mapChunkManager.set_map(x,y,z) #tells to redraw chunk
        self.mapController.incoming_map_chunk(x,y,z)

    def _set_map(self, value, **msg):
        (x,y,z,value) = value
        self.terrainMap.set(x,y,z,value)
        self.mapChunkManager.set_map(x,y,z) #redraw chunk


from game_state import GameStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_chunk_manager import MapChunkManagerGlobal
from map_controller import MapControllerGlobal
from input import InputGlobal
