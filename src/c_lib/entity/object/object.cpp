#include "object.hpp"

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <entity/network/packets.hpp>
#include <entity/components/draw/billboard_sprite.hpp>
#include <entity/components/pickup.hpp>
#include <entity/components/health/ttl.hpp>

namespace Objects
{

using Components::Component;

void Object::add_component(int slot, class Component* component)
{
    GS_ASSERT(slot >= 0);
    if (slot < 0) return;
    GS_ASSERT(slot < this->n_components);
    if (slot >= this->n_components) return;
    GS_ASSERT(this->components[slot] == NULL);
    this->components[slot] = component;
}

// returns component of type if available
// must cast to desired component manually
class Component* Object::get_component(ComponentType type)
{
    int slot = object_data->get_component_slot(this->type, type);
    if (slot < 0) return NULL;
    GS_ASSERT(slot < this->n_components);
    return this->components[slot];
}

class Component* Object::get_component_interface(ComponentInterfaceType interface)
{
    int slot = object_data->get_component_interface_slot(this->type, interface);
    if (slot < 0) return NULL;
    GS_ASSERT(slot < this->n_components);
    return this->components[slot];
}

void Object::broadcastDeath()
{
    object_destroy_StoC msg;
    msg.id = this->id;
    msg.type = this->type;
    msg.broadcast();
}

void Object::init(int n_components)
{
    GS_ASSERT(this->components == NULL);
    GS_ASSERT(this->n_components == 0);
    GS_ASSERT(n_components >= 0);
    if (n_components < 0) n_components = 0;
    this->n_components = n_components;
    if (n_components <= 0) return;
    this->components = (class Component**)calloc(n_components, sizeof(class Component*));
}

Object::~Object()
{
    if (this->components != NULL) free(this->components);
}

} // Objects
