#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionPhysicsComponent: public PhysicsComponent
{
    protected:
        Vec3 position;
        Vec3 angles;
        bool changed;

        #if DC_CLIENT
        // interpolation
        int tick;
        int previous_tick;
        Vec3 previous_position;
        Vec3 computed_position;
        #endif

    public:

    void set_changed(bool changed)
    {
        this->changed = changed;
    }

    bool get_changed()
    {
        return this->changed;
    }

    Vec3 get_position()
    {
        #if DC_SERVER
        return this->position;
        #endif
        #if DC_CLIENT
        return this->computed_position;
        #endif
    }

    bool set_position(Vec3 position)
    {
        position = translate_position(position);
        #if DC_CLIENT
        this->previous_position = this->position;
        this->previous_tick = this->tick;
        #endif
        if (vec3_equal(this->position, position))
            return false;
        this->position = position;
        #if DC_SERVER
        this->changed = true;
        #endif
        return true;
    }

    Vec3 get_momentum()
    {
        return NULL_MOMENTUM;
    }

    bool set_momentum(Vec3 momentum)
    {
        return false;
    }

    Vec3 get_angles()
    {
        return this->angles;
    }

    bool set_angles(Vec3 angles)
    {
        if (vec3_equal(this->angles, angles))
            return false;
        this->angles = angles;
        this->changed = true;
        return true;
    }

    #if DC_CLIENT
    void call()
    {
        float delta = float(this->tick - this->previous_tick) / float(MOB_BROADCAST_RATE);
        Vec3 end = quadrant_translate_position(this->previous_position, this->position);
        Vec3 new_position = vec3_interpolate(this->previous_position, end, delta);
        new_position = translate_position(new_position);
        if (vec3_equal(new_position, this->computed_position))
        {
            this->tick++;
            return;
        }
        this->computed_position = new_position;
        this->changed = true;
        GS_ASSERT(vec3_is_valid(this->computed_position));
        GS_ASSERT(is_boxed_position(this->computed_position));
        this->tick++;
    }
    #endif

    PositionPhysicsComponent() :
        PhysicsComponent(COMPONENT_POSITION)
    {
        this->_init();
    }

    explicit PositionPhysicsComponent(ComponentType type) :
        PhysicsComponent(type)
    {
        this->_init();
    }

    private:
    void _init()
    {
        this->position = NULL_POSITION;
        this->angles = NULL_ANGLES;
        this->changed = true;

        #if DC_CLIENT
        this->tick = 0;
        this->previous_tick = 0;
        this->previous_position = NULL_POSITION;
        this->computed_position = NULL_POSITION;
        #endif
    }
};

} // Components
