#pragma once

namespace Toolbelt
{

// Common
extern bool* agent_fire_on;
extern int* agent_fire_cooldown;
extern ItemType* agent_selected_type;
extern ItemType fist_item_type;    // cached item type lookup

// Client
#if DC_CLIENT
extern int selected_slot;
extern ItemContainerID toolbelt_id;
extern bool single_trigger; // last event was a single trigger event
extern bool holding;        // is holding button down
extern int holding_tick;    // how long we've been holding down
extern bool charging;       // player is charging their weapon. it is different
                            // from holding in that the player is only considered
                            // charging if the weapon supports charging and
                            // they've been holding longer than some threshold
#endif

// Server
#if DC_SERVER
extern int* agent_selected_slot;
extern ItemID* agent_selected_item;
#endif

void init_state();
void teardown_state();

}    // Toolbelt
