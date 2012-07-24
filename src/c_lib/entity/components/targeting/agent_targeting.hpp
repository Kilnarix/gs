#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class AgentTargetingComponent: public TargetingComponent
{
    public:
        float speed;
        int max_z_diff;

        Vec3 destination;
        bool at_destination;
        bool en_route;
        int ticks_to_destination;

        int max_lock_ticks;
        int ticks_locked;

        void check_target_alive();

        // explicitly set the target
        void set_target(int agent_id);

        // lock to agent
        void lock_target(Vec3 camera_position);

        void orient_to_target(Vec3 camera_position);

        bool move_on_surface();

        void broadcast_target_choice();
        void broadcast_remove_target();

        void call();
    
    virtual ~AgentTargetingComponent() {}
    
    AgentTargetingComponent()
    : TargetingComponent(COMPONENT_AGENT_TARGETING),
    speed(1.0f), max_z_diff(128),
    destination(vec3_init(0,0,0)), at_destination(false), en_route(false), ticks_to_destination(1),
    max_lock_ticks(0), ticks_locked(0)
    {}
};

} // Components
