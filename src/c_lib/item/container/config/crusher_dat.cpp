#include "crusher_dat.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/config/item_drop.hpp>

namespace ItemContainer
{

float crusher_item_jump_out_velocity()
{
    static const float m = 1.5f;
    return ((randf()*0.5f) + 1.0f) * m;
}

static class Item::ItemDrop* drops;

/* Configuration Loader */ 
    
static class Item::ItemDrop* d = NULL;

static void set_crusher_drop(int item_type)
{
    GS_ASSERT_ABORT(drops != NULL);
    if (drops == NULL) return;

    ASSERT_VALID_ITEM_TYPE(item_type);
    IF_INVALID_ITEM_TYPE(item_type)
    {
        GS_ASSERT_ABORT(false);
        return;
    }

    GS_ASSERT_ABORT(!drops[item_type].is_loaded());
    if (drops[item_type].is_loaded()) return;
    d = &drops[item_type];
}

static void crusher_def(const char* name)
{
    int item_type = Item::get_item_type(name);
    GS_ASSERT_ABORT(item_type != NULL_ITEM_TYPE);
    set_crusher_drop(item_type);
}

static void register_crusher_settings()
{    
    crusher_def("regolith");
    d->set_max_drop_types(6);
    
    d->set_max_drop_amounts("powdered_regolith", 3);
    d->add_drop("powdered_regolith", 1, 0.85f);
    d->add_drop("powdered_regolith", 2, 0.12f);
    d->add_drop("powdered_regolith", 3, 0.03f);

    d->set_max_drop_amounts("copper_ore", 1);
    d->add_drop("copper_ore", 1, 0.005f);
    
    d->set_max_drop_amounts("iron_ore", 1);
    d->add_drop("iron_ore", 1, 0.005f);

    d->set_max_drop_amounts("gallium_ore", 1);
    d->add_drop("gallium_ore", 1, 0.0025f);

    d->set_max_drop_amounts("iridium_ore", 1);
    d->add_drop("iridium_ore", 1, 0.0025f);

    d->set_max_drop_amounts("coal", 1);
    d->add_drop("coal", 1, 0.005f);

    crusher_def("iron_rod");
    d->set_max_drop_types(1);
    d->set_max_drop_amounts("iron_bar", 1);
    d->add_drop("iron_bar", 1, 1.0f);

    crusher_def("iron_star");
    d->set_max_drop_types(1);
    d->set_max_drop_amounts("iron_bar", 1);
    d->add_drop("iron_bar", 1, 1.0f);
    
    crusher_def("iron_blade");
    d->set_max_drop_types(1);
    d->set_max_drop_amounts("iron_bar", 1);
    d->add_drop("iron_bar", 1, 1.0f);

    crusher_def("copper_helmet");
    d->set_max_drop_types(1);
    d->set_max_drop_amounts("copper_bar", 1);
    d->add_drop("copper_bar", 1, 1.0f);

    crusher_def("iron_helmet");
    d->set_max_drop_types(1);
    d->set_max_drop_amounts("iron_bar", 1);
    d->add_drop("iron_bar", 1, 1.0f);

    crusher_def("gallium_helmet");
    d->set_max_drop_types(1);
    d->set_max_drop_amounts("gallium_bar", 1);
    d->add_drop("gallium_bar", 1, 1.0f);

    crusher_def("iridium_helmet");
    d->set_max_drop_types(1);
    d->set_max_drop_amounts("iridium_bar", 1);
    d->add_drop("iridium_bar", 1, 1.0f);    
}

void validate_crusher_settings()
{
}

void load_crusher_dat()
{
    register_crusher_settings();
    validate_crusher_settings();
}


/* Public Interface */

void init_crusher_dat()
{
    GS_ASSERT_ABORT(drops == NULL);
    drops = new class Item::ItemDrop[MAX_ITEM_TYPES];
}

void teardown_crusher_dat()
{
    if (drops != NULL) delete[] drops;
}

class Item::ItemDrop* get_crusher_drop(int item_type)
{
    ASSERT_VALID_ITEM_TYPE(item_type);
    IF_INVALID_ITEM_TYPE(item_type) return NULL;
    return &drops[item_type];
}

}   // ItemContainer
