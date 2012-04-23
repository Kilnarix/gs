#pragma once

namespace Objects
{

// forward decl
class Object;

void load_mob_spawner_data();
Object* create_mob_spawner();
void ready_mob_spawner(Object* object);
void die_mob_spawner(Object* object);
void tick_mob_spawner(Object* object);
void update_mob_spawner(Object* object);

} // Objects

