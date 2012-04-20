#include "billboard_sprite.hpp"

#if DC_CLIENT

#include <c_lib/draw/draw.hpp>
#include <c_lib/entity/components/physics.hpp>

namespace Components
{

void BillboardSpriteComponent::call()
{
    #if DC_CLIENT
    PhysicsComponent* physics = (PhysicsComponent*)this->object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics == NULL) return;
    drawBillboardSprite(physics->get_position(), this->sprite_index, this->scale);
    #endif
}

} // Components

#endif
