#pragma once

#include <hud/input.hpp>

namespace Hud
{

class Form
{
    protected:
        InputBox** inputs;
        int input_index;
        int n_inputs;
        int focused;

    #define REGISTER_INPUT(TYPE, LABEL) \
        TYPE* register_##LABEL##_input(const char* name) \
        { \
            IF_ASSERT(this->input_index >= this->n_inputs) return NULL; \
            TYPE* input = new TYPE; \
            this->inputs[this->input_index] = input; \
            input->set_name(name); \
            if (this->input_index == 0) \
                input->focus(); \
            this->input_index++; \
            return input; \
        }

    REGISTER_INPUT(Button, button)
    REGISTER_INPUT(InputCheckBox, checkbox)
    REGISTER_INPUT(InputTextBox, text)

    #undef REGISTER_INPUT

    void check_all_loaded()
    {
        for (int i=0; i<this->n_inputs; i++)
            GS_ASSERT(this->inputs[i] != NULL);
    }

    virtual void draw_label()
    {
        if (!this->n_inputs) return;
        this->label.set_text(this->name());
        this->label.set_color(Color(180, 220, 180));
        Vec2 p = this->position;
        p.y += this->label.get_height();
        p.y += this->inputs[0]->dims.y + this->inputs[0]->border.y * 0.5f;
        p.x += (this->inputs[0]->dims.x - this->label.get_width()) * 0.5f;
        this->label.set_position(p);
        this->label.draw();
    }

    public:

        Vec2 position;
        HudText::Text label;

    void cycle()
    {
        this->focused = (this->focused + 1) % this->n_inputs;
    }

    InputBox* get_focused_input()
    {
        IF_ASSERT(this->focused < 0 || this->focused >= this->n_inputs)
            return NULL;
        return this->inputs[this->focused];
    }

    void save()
    {
        // write entered data to text file
    }

    void draw()
    {
        for (int i=0; i<this->n_inputs; i++)
            if (this->inputs[i] != NULL)
                this->inputs[i]->draw();
    }

    void draw_text()
    {
        for (int i=0; i<this->n_inputs; i++)
            if (this->inputs[i] != NULL)
                this->inputs[i]->draw_text();
        this->draw_label();
    }

    void set_position(const Vec2& position)
    {
        this->position = position;
        Vec2 p = position;
        for (int i=0; i<this->n_inputs; i++)
        {
            this->inputs[i]->set_position(p);
            p.y -= this->inputs[i]->dims.y;
        }
    }

    virtual const char* name() = 0;
    virtual void submit() = 0;

    explicit Form(int n_inputs) :
        inputs(NULL), input_index(0), n_inputs(n_inputs), focused(0),
        position(vec2_init(0))
    {
        IF_ASSERT(this->n_inputs <= 0) return;
        this->inputs = (InputBox**)calloc(this->n_inputs, sizeof(*this->inputs));
    }

    virtual ~Form()
    {
        if (this->inputs != NULL)
            for (int i=0; i<this->n_inputs; i++)
                delete this->inputs[i];
        free(this->inputs);
    }
};

class LoginForm: public Form
{
    public:
        InputTextBox* username;
        InputTextBox* password;
        InputCheckBox* remember;
        Button* submit_button;

    virtual const char* name()
    {
        return "Login";
    }

    virtual void submit()
    {
        for (int i=0; i<this->n_inputs; i++)
            printf("%s: %s\n", this->inputs[i]->get_name(), this->inputs[i]->get_value());
    }

    LoginForm() :
        Form(4)
    {
        this->username = (InputTextBox*)this->register_text_input("username");
        this->password = (InputTextBox*)this->register_text_input("password");
        this->password->password = true;
        this->remember = (InputCheckBox*)this->register_checkbox_input("remember");
        this->submit_button = (Button*)this->register_button_input("submit");
        this->check_all_loaded();

        this->username->set_text("rdn");
        this->password->set_text("password");
    }

    virtual ~LoginForm() {}
};

class CreateAccountForm: public Form
{
    public:
        InputTextBox* username;
        InputTextBox* email;
        InputTextBox* password;
        InputCheckBox* remember;
        Button* submit_button;

    virtual const char* name()
    {
        return "Create Account";
    }

    virtual void submit()
    {
        for (int i=0; i<this->n_inputs; i++)
            printf("%s: %s\n", this->inputs[i]->get_name(), this->inputs[i]->get_value());
    }

    CreateAccountForm() :
        Form(5)
    {
        this->username = (InputTextBox*)this->register_text_input("username");
        this->email = (InputTextBox*)this->register_text_input("email");
        this->password = (InputTextBox*)this->register_text_input("password");
        this->password->password = true;
        this->remember = (InputCheckBox*)this->register_checkbox_input("remember");
        this->submit_button = (Button*)this->register_button_input("submit");
        this->check_all_loaded();
    }

    virtual ~CreateAccountForm() {}
};

}   // Hud
