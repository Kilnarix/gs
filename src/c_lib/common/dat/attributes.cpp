#include "attributes.hpp"

#include <common/dat/properties.hpp>

namespace Attributes
{

class Attribute: public Property<AttributeType>
{
    public:
        AttributeGroup group;
        AttributeValueType value_type;
        AttributeSyncType sync_type;

        void* value;

        voidFunction get_callback;
        voidFunction set_callback;

        bool changed;
        ClientID sync_to;

    /* Read/write API */

    int get_int() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return 1;
        int s = *(reinterpret_cast<int*>(this->value));
        if (this->get_callback != NULL)
            return reinterpret_cast<getInt>(this->get_callback)(s);
        return s;
    }

    float get_float() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return 1.0f;
        float s = *(reinterpret_cast<float*>(this->value));
        if (this->get_callback != NULL)
            return reinterpret_cast<getFloat>(this->get_callback)(s);
        return s;
    }

    const char* get_string() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return NULL;
        char* s = *(reinterpret_cast<char**>(this->value));
        if (this->get_callback != NULL)
            return reinterpret_cast<getString>(this->get_callback)(s);
        return s;
    }

    bool set(int value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return false;
        int old = *(reinterpret_cast<int*>(this->value));
        if (this->set_callback != NULL)
            value = reinterpret_cast<setInt>(this->set_callback)(old, value);
        if (old != value)
        {
            *(reinterpret_cast<int*>(this->value)) = value;
            return true;
        }
        return false;
    }

    bool set(float value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return false;
        float old = *(reinterpret_cast<float*>(this->value));
        if (this->set_callback != NULL)
            value = reinterpret_cast<setFloat>(this->set_callback)(old, value);
        if (old != value)
        {
            *(reinterpret_cast<float*>(this->value)) = value;
            return true;
        }
        return false;
    }

    bool set(const char* value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return false;
        char* old = *(reinterpret_cast<char**>(this->value));
        if (this->set_callback != NULL)
            value = reinterpret_cast<setString>(this->set_callback)(old, value);
        if (strcmp(old, value) != 0)
        {
            char* loc = *(reinterpret_cast<char**>(this->value));
            strncpy(loc, value, STRING_ATTRIBUTE_MAX_LENGTH);
            loc[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
            return true;
        }
        return false;
    }

    /* Registration API */

    void add_get_callback(getInt get_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_get_callback(reinterpret_cast<voidFunction>(get_callback));
    }

    void add_get_callback(getFloat get_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_get_callback(reinterpret_cast<voidFunction>(get_callback));
    }

    void add_get_callback(getString get_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_get_callback(reinterpret_cast<voidFunction>(get_callback));
    }

    void add_set_callback(setInt set_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_set_callback(reinterpret_cast<voidFunction>(set_callback));
    }

    void add_set_callback(setFloat set_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_set_callback(reinterpret_cast<voidFunction>(set_callback));
    }

    void add_set_callback(setString set_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_set_callback(reinterpret_cast<voidFunction>(set_callback));
    }

    void set_sync_type(AttributeSyncType sync_type)
    {
        GS_ASSERT(this->sync_type == NULL_ATTRIBUTE_SYNC_TYPE);
        this->sync_type = sync_type;
    }

    void set_value_type(AttributeValueType value_type)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE);
        this->value_type = value_type;
        switch (value_type)
        {
            case ATTRIBUTE_VALUE_INT:
                this->value = calloc(1, sizeof(int));
                break;
            case ATTRIBUTE_VALUE_FLOAT:
                this->value = calloc(1, sizeof(float));
                break;
            case ATTRIBUTE_VALUE_STRING:
                this->value = calloc(STRING_ATTRIBUTE_MAX_LENGTH+1, sizeof(char));
                break;
            case NULL_ATTRIBUTE_VALUE_TYPE:
                this->value = NULL;
                GS_ASSERT(false);
                break;
        }
    }

    /* Networking */
    #if DC_SERVER
    void set_sync_to(ClientID client_id)
    {
        GS_ASSERT(this->sync_to == NULL_CLIENT);
        GS_ASSERT(isValid(client_id));
        this->sync_to = client_id;
    }

    void send_changes()
    {
        if (!this->changed) return;
        if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PRIVATE) return;

        #define ROUTE_MESSAGE(MSG_TYPE) \
        { do { \
            MSG_TYPE msg; \
            this->_pack_message(&msg); \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_ALL) \
                msg.broadcast(); \
            else \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PLAYER) \
            { \
                GS_ASSERT(this->sync_to != NULL_CLIENT); \
                msg.sendToClient(this->sync_to); \
            } \
        } while (0); }

        if (this->value_type == ATTRIBUTE_VALUE_INT)
        {
            ROUTE_MESSAGE(set_attribute_int_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_FLOAT)
        {
            ROUTE_MESSAGE(set_attribute_float_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_STRING)
        {
            ROUTE_MESSAGE(set_attribute_string_StoC);
        }
        else
        {
            GS_ASSERT(false);
        }
        #undef ROUTE_MESSAGE
        this->changed = false;
    }

    void send_to_client(ClientID client_id) const
    {
        IF_ASSERT(!isValid(client_id)) return;
        if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PRIVATE) return;

        #define ROUTE_MESSAGE(MSG_TYPE) \
        { do { \
            MSG_TYPE msg; \
            this->_pack_message(&msg); \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_ALL) \
                msg.sendToClient(client_id); \
            else \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PLAYER) \
            { \
                GS_ASSERT(this->sync_to == client_id); \
                msg.sendToClient(this->sync_to); \
            } \
        } while (0); }

        if (this->value_type == ATTRIBUTE_VALUE_INT)
        {
            ROUTE_MESSAGE(set_attribute_int_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_FLOAT)
        {
            ROUTE_MESSAGE(set_attribute_float_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_STRING)
        {
            ROUTE_MESSAGE(set_attribute_string_StoC);
        }
        else
        {
            GS_ASSERT(false);
        }
        #undef ROUTE_MESSAGE
    }
    #endif

    /* Verification */

    void verify() const
    {
        GS_ASSERT(this->type != NULL_ATTRIBUTE);
        GS_ASSERT(this->group != NULL_ATTRIBUTE_GROUP);
        GS_ASSERT(this->value_type != NULL_ATTRIBUTE_VALUE_TYPE);
        GS_ASSERT(this->name[0] != '\0');
        GS_ASSERT(this->value != NULL);
        #if DC_SERVER
        GS_ASSERT(this->sync_type != NULL_ATTRIBUTE_SYNC_TYPE);
        GS_ASSERT(this->sync_type != ATTRIBUTE_SYNC_TYPE_PLAYER ||
                  this->sync_to != NULL_CLIENT);
        #endif
    }

    void verify_other(const Attribute* other) const
    {
        GS_ASSERT(this->get_callback == NULL || this->get_callback != other->get_callback);
        GS_ASSERT(this->set_callback == NULL || this->set_callback != other->set_callback);
        GS_ASSERT(this->value != other->value);
        GS_ASSERT(strcmp(this->name, other->name) != 0);
    }

    Attribute() :
        Property<AttributeType>(NULL_ATTRIBUTE),
        group(NULL_ATTRIBUTE_GROUP),
        value_type(NULL_ATTRIBUTE_VALUE_TYPE),
        sync_type(NULL_ATTRIBUTE_SYNC_TYPE),
        value(NULL), get_callback(NULL), set_callback(NULL),
        changed(false), sync_to(NULL_CLIENT)
    {
    }

    ~Attribute()
    {
        if (this->value != NULL) free(this->value);
    }

    private:

    void _add_set_callback(voidFunction set_callback)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->set_callback != NULL);
        this->set_callback = set_callback;
    }

    void _add_get_callback(voidFunction get_callback)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->get_callback != NULL);
        this->get_callback = get_callback;
    }

    #if DC_SERVER
    void _pack_message(set_attribute_int_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_INT);
        msg->attribute_group = this->group;
        msg->attribute_type = this->type;
        msg->value = this->get_int();
    }

    void _pack_message(set_attribute_float_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_FLOAT);
        msg->attribute_group = this->group;
        msg->attribute_type = this->type;
        msg->value = this->get_float();
    }

    void _pack_message(set_attribute_string_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_STRING);
        msg->attribute_group = this->group;
        msg->attribute_type = this->type;
        strncpy(msg->value, this->get_string(), STRING_ATTRIBUTE_MAX_LENGTH);
        msg->value[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
    }
    #endif
};


class Attributes: public Properties<Attribute, AttributeType>
{
    public:
        AttributeGroup group;

    void verify() const
    {
        // attributes should be laid out continuously from 0, although properties allows otherwise
        bool loaded = this->properties[0].loaded;
        int switch_ct = 0;
        GS_ASSERT(this->index == 0 || loaded);
        for (size_t i=0; i<this->max; i++)
        {
            if (this->properties[i].loaded)
                loaded = true;
            else
            {
                if (loaded) switch_ct++;
                loaded = false;
            }
        }
        GS_ASSERT(switch_ct <= 1);

        for (size_t i=0; i<this->max; i++)
            if (this->properties[i].loaded)
                this->properties[i].verify();

        for (size_t i=0; i<this->max-1; i++)
        for (size_t j=i+1; j<this->max; j++)
            if (this->properties[i].loaded && this->properties[j].loaded)
                this->properties[i].verify_other(&this->properties[j]);
    }



    #if DC_SERVER
    void set_sync_to(ClientID client_id)
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded &&
                this->properties[i].sync_type == ATTRIBUTE_SYNC_TYPE_PLAYER)
                    this->properties[i].set_sync_to(client_id);
    }

    void send_to_client(ClientID client_id) const
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send_to_client(client_id);
    }

    void send_changes()
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send_changes();
    }
    #endif

    void done_loading()
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].group = this->group;
        Properties<Attribute, AttributeType>::done_loading();
    }

    Attributes() :
        Properties<Attribute, AttributeType>(MAX_ATTRIBUTES)
    {}
};


class AttributesManager
{
    public:

        size_t count;
        size_t max;

        Attributes** attributes;

    Attributes* create()
    {
        IF_ASSERT(this->count >= this->max) return NULL;
        Attributes* a = new Attributes;
        a->group = (AttributeGroup)this->count;
        this->attributes[this->count++] = a;
        return a;
    }

    Attributes* get(AttributeGroup group)
    {
        IF_ASSERT(!isValid(group)) return NULL;
        return this->attributes[group];
    }

    void verify() const
    {
        for (size_t i=0; i<this->count; i++)
            this->attributes[i]->verify();
    }

    #if DC_SERVER
    void send_changes()
    {
        for (size_t i=0; i<this->count; i++)
            this->attributes[i]->send_changes();
    }
    #endif

    AttributesManager(size_t max) :
        count(0), max(max)
    {
        this->attributes = new Attributes*[max];
    }

    ~AttributesManager()
    {
        for (size_t i=0; i<this->count; i++)
            delete this->attributes[i];
        delete[] this->attributes;
    }
};

static AttributesManager* attributes_manager;

/* Read/Write API */

bool set(AttributeGroup group, AttributeType type, int value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, AttributeType type, float value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, AttributeType type, const char* value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, const char* name, int value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, const char* name, float value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, const char* name, const char* value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

int get_int(AttributeGroup group, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return 1;
    return a->get_int();
}

float get_float(AttributeGroup group, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1.0f;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return 1.0f;
    return a->get_float();
}

const char* get_string(AttributeGroup group, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return NULL;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return NULL;
    return a->get_string();
}

int get_int(AttributeGroup group, const char* name)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return 1;
    return a->get_int();
}

float get_float(AttributeGroup group, const char* name)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1.0f;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return 1.0f;
    return a->get_float();
}

const char* get_string(AttributeGroup group, const char* name)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return NULL;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return NULL;
    return a->get_string();
}

/* Boilerplate */

void init()
{
    GS_ASSERT(attributes_manager == NULL);
    attributes_manager = new AttributesManager(MAX_ATTRIBUTE_GROUPS);
}

void teardown()
{
    if (attributes_manager != NULL) delete attributes_manager;
}

void verify()
{
    attributes_manager->verify();
}

/* Meta API */

const char* get_name(AttributeGroup group, AttributeType type)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL;
    Attribute* a = attr->get(type);
    IF_ASSERT(a == NULL) return NULL;
    return a->name;
}

AttributeType get_type(AttributeGroup group, const char* name)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL_ATTRIBUTE;
    Attribute* a = attr->get(name);
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE;
    return a->type;
}

size_t get_attribute_count(AttributeGroup group)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL_ATTRIBUTE;
    // Attributes are guaranteed continuous in the property array,
    // so its safe to return the index
    return attr->index;
}

AttributeValueType get_value_type(AttributeGroup group, const char* name)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    Attribute* a = attr->get(name);
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    return a->value_type;
}

AttributeValueType get_value_type(AttributeGroup group, AttributeType type)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    Attribute* a = attr->get(type);
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    return a->value_type;
}

/* Registration API */

static Attributes* current_attributes;

// value trackers to allow late-loading of defaults
static AttributeType _type = NULL_ATTRIBUTE;
static AttributeValueType _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
static int _ival;
static float _fval;
static char _sval[STRING_ATTRIBUTE_MAX_LENGTH+1];

static void set_value_type(AttributeType type, AttributeValueType value_type)
{
    IF_ASSERT(current_attributes == NULL) return;
    Attribute* a = current_attributes->get(type);
    IF_ASSERT(a == NULL) return;
    a->set_value_type(value_type);
}

static void _set_saved()
{
    IF_ASSERT(current_attributes == NULL) return;
    current_attributes->done_loading();
    switch (_vtype)
    {
        case ATTRIBUTE_VALUE_INT:
            set_value_type(_type, ATTRIBUTE_VALUE_INT);
            set(current_attributes->group, _type, _ival);
            _ival = 0;
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case ATTRIBUTE_VALUE_FLOAT:
            set_value_type(_type, ATTRIBUTE_VALUE_FLOAT);
            set(current_attributes->group, _type, _fval);
            _fval = 0.0f;
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case ATTRIBUTE_VALUE_STRING:
            set_value_type(_type, ATTRIBUTE_VALUE_STRING);
            set(current_attributes->group, _type, _sval);
            _sval[0] = '\0';
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case NULL_ATTRIBUTE_VALUE_TYPE:
            GS_ASSERT(false);
            break;
    }

    // unset changed; it should be false at init
    Attribute* a = current_attributes->get(_type);
    a->changed = false;

    _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
}

AttributeGroup start_registration()
{
    GS_ASSERT(current_attributes == NULL);
    current_attributes = attributes_manager->create();
    IF_ASSERT(current_attributes == NULL) return NULL_ATTRIBUTE_GROUP;
    return current_attributes->group;
}

void end_registration()
{
    done_loading();
    current_attributes = NULL;
}

static AttributeType def(const char* name)
{
    IF_ASSERT(current_attributes == NULL) return NULL_ATTRIBUTE;
    if (_vtype != NULL_ATTRIBUTE_VALUE_TYPE) _set_saved();
    Attribute* a = current_attributes->get_next();
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE;
    a->set_name(name);
    return a->type;
}

AttributeType def(const char* name, int value)
{
    AttributeType type = def(name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    _ival = value;
    _vtype = ATTRIBUTE_VALUE_INT;
    _type = type;
    return type;
}

AttributeType def(const char* name, float value)
{
    AttributeType type = def(name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    _fval = value;
    _vtype = ATTRIBUTE_VALUE_FLOAT;
    _type = type;
    return type;
}

AttributeType def(const char* name, const char* value)
{
    AttributeType type = def(name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    strncpy(_sval, value, STRING_ATTRIBUTE_MAX_LENGTH+1);
    _sval[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
    _vtype = ATTRIBUTE_VALUE_STRING;
    _type = type;
    return type;
}

#define UNPACK_CURRENT_ATTRIBUTE \
    IF_ASSERT(current_attributes == NULL) return; \
    Attribute* a = current_attributes->_get_any(type); \
    IF_ASSERT(a == NULL) return;

void set_sync_type(AttributeType type, AttributeSyncType sync_type)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_sync_type(sync_type);
}

void add_get_callback(AttributeType type, getInt cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_get_callback(cb);
}

void add_get_callback(AttributeType type, getFloat cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_get_callback(cb);
}

void add_get_callback(AttributeType type, getString cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_get_callback(cb);
}

void add_set_callback(AttributeType type, setInt cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_set_callback(cb);
}

void add_set_callback(AttributeType type, setFloat cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_set_callback(cb);
}

void add_set_callback(AttributeType type, setString cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_set_callback(cb);
}

#undef UNPACK_CURRENT_ATTRIBUTE

void done_loading()
{
    IF_ASSERT(current_attributes == NULL) return;
    if (_vtype != NULL_ATTRIBUTE_VALUE_TYPE) _set_saved();
    current_attributes->done_loading();
}

/* Network */

void init_packets()
{
    set_attribute_int_StoC::register_client_packet();
    set_attribute_float_StoC::register_client_packet();
    set_attribute_string_StoC::register_client_packet();
}

#if DC_SERVER
void set_sync_to(AttributeGroup group, ClientID client_id)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return;
    attr->set_sync_to(client_id);
}

void send_to_client(AttributeGroup group, ClientID client_id)
{
    Attributes* a = attributes_manager->get(group);
    IF_ASSERT(a == NULL) return;
    a->send_to_client(client_id);
}

void send_changes()
{
    attributes_manager->send_changes();
}
#endif

#if DC_CLIENT
inline void set_attribute_int_StoC::handle()
{
    AttributeGroup group = (AttributeGroup)this->attribute_group;
    AttributeType type = (AttributeType)this->attribute_type;
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get(type);
    IF_ASSERT(attr == NULL) return;
    attr->set((int)this->value);

    printf("%s set to %d\n", get_name(group, type), get_int(group, type));
}

inline void set_attribute_float_StoC::handle()
{
    AttributeGroup group = (AttributeGroup)this->attribute_group;
    AttributeType type = (AttributeType)this->attribute_type;
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get(type);
    IF_ASSERT(attr == NULL) return;
    attr->set(this->value);
}

inline void set_attribute_string_StoC::handle()
{
    AttributeGroup group = (AttributeGroup)this->attribute_group;
    AttributeType type = (AttributeType)this->attribute_type;
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get(type);
    IF_ASSERT(attr == NULL) return;
    attr->set(this->value);
}
#endif

#if DC_SERVER
inline void set_attribute_int_StoC::handle() {}
inline void set_attribute_float_StoC::handle() {}
inline void set_attribute_string_StoC::handle() {}
#endif

}   // Attributes
