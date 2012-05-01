#pragma once

// Common
namespace Toolbelt
{

void init();
void teardown();

} // Toolbelt

#if DC_CLIENT
namespace Toolbelt
{

// network events
void assign_toolbelt(int container_id);

// UI events
void toolbelt_item_selected_event(int container_id, int slot);
void left_trigger_event();
void right_trigger_event();
void reload_event();

} // Toolbelt
#endif

#if DC_SERVER
namespace Toolbelt
{

ItemID get_agent_selected_item(int agent_id);
void set_agent_toolbelt_slot(int agent_id, int slot);

} // Toolbelt
#endif
