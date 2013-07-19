#pragma once

#include <physics/vec2.hpp>
#include <common/color.hpp>
#include <hud/text.hpp>
#include <hud/font.hpp>

namespace Hud
{

class InputBox
{
    public:
        HudText::Text label;
        int label_margin;
        bool focused;
        Vec2 position;
        Vec2 border;
        Vec2 dims;
        Color label_color;
        Color border_color;
        Color background_color;

    virtual void hover(const Vec2& cursor) {}
    virtual void click(const Vec2& cursor) {}

    void focus()
    {
        this->focused = true;
    }

    void unfocus()
    {
        this->focused = false;
    }

    bool contains_point(const Vec2& point)
    {
        return point_in_rect(point, this->position, this->dims);
    }

    void set_position(const Vec2& position)
    {
        this->position = position;
    }

    void set_dimensions(const Vec2& dims)
    {
        this->dims = dims;
    }

    void set_border(int border)
    {
        this->border = vec2_init(border);
    }

    virtual const char* get_value() = 0;
    void set_name(const char* name)
    {
        strncpy(this->name, name, this->name_max_length);
        this->name[this->name_max_length] = '\0';
    }

    const char* get_name()
    {
        return this->name;
    }

    virtual void draw() = 0;

    virtual void draw_text()
    {
        this->draw_label();
    }

    virtual void draw_label()
    {
        if (HudFont::font == NULL) return;
        const char* _name = this->get_name();
        if (_name[0] == '\0') return;
        static char name[name_max_length + 1];
        strcpy(name, _name);
        size_t colon_pos = strlen(name);
        if (colon_pos == name_max_length)
            colon_pos -= 1;
        name[colon_pos++] = ':';
        name[colon_pos] = '\0';

        int width = 0;
        HudFont::font->get_string_pixel_dimension(name, &width, NULL);
        Vec2 p = this->position;
        p.x -= width + this->label_margin + this->border.x;
        p.y += this->border.y;
        p.y += this->label.get_height();

        this->label.set_position(p);
        this->label.set_text(name);
        this->label.set_color(Color(200, 200, 255));
        this->label.draw();
    }

    InputBox() :
        label_margin(2), focused(false),
        position(vec2_init(0)), border(vec2_init(8)), dims(vec2_init(200, 30)),
        border_color(Color(10, 150, 50)),
        background_color(Color(0, 100, 10, 64))
    {
        memset(this->name, 0, sizeof(this->name));
    }

    virtual ~InputBox() {}

    protected:
        static const size_t name_max_length = 63;
        char name[name_max_length + 1];

    virtual void draw_border()
    {
        draw_border_rect(this->border_color, this->position, this->dims, this->border.x);
    }

    virtual void draw_background()
    {
        draw_rect(this->background_color, this->position, this->dims);
    }
};

class InputCheckBox: public InputBox
{
    public:
        bool checked;

    virtual void click(const Vec2& cursor)
    {
        if (!this->contains_point(cursor)) return;
        this->toggle();
    }

    void toggle()
    {
        this->checked = !this->checked;
    }

    virtual const char* get_value()
    {
        if (this->checked)
            return "t";
        else
            return "f";
    }

    virtual void draw()
    {
        this->draw_background();
        this->draw_border();
        this->draw_checkbox();
    }

    virtual void draw_text()
    {
        this->draw_checkbox_text();
        this->draw_label();
    }

    InputCheckBox() :
        InputBox(), checked(false)
    {
        this->background_color.a = 0xFF;
    }

    virtual ~InputCheckBox() {}

    protected:

        HudText::Text checked_text;

    void draw_checkbox_text()
    {
        if (this->checked)
        {
            this->checked_text.set_text("yes");
            this->checked_text.set_color(Color(224, 64, 64));
        }
        else
        {
            this->checked_text.set_text("no");
            this->checked_text.set_color(Color(96, 96, 96));
        }
        Vec2 p = this->position;
        p.x += this->dims.x * 0.5f;
        p.x -= this->checked_text.get_width() * 0.5f;
        p.y += this->checked_text.get_height() + this->border.y;
        this->checked_text.set_position(p);
        this->checked_text.draw();
    }

    void draw_checkbox()
    {
        Vec2 p = this->position;
        p.x += dims.x * 0.5f;
        int width = 0;
        HudFont::font->get_string_pixel_dimension("-yes-", &width, NULL);
        Vec2 d;
        d.x = width;
        d.y = this->dims.y - this->border.y * 0.5f;
        p.x -= d.x * 0.5f;
        draw_rect(Color(32, 32, 32, 128), p, d);
    }
};

class InputTextBox: public InputBox
{
    public:

        HudText::Text text;
        int cursor;
        int cursor_width;
        Color text_color;
        Color cursor_color;
        bool password;

    virtual const char* get_value()
    {
        return this->text.text;
    }

    const char* get_text() const
    {
        return this->text.text;
    }

    void set_cursor(int x)
    {
        x = GS_MIN(x, int(this->text.length()));
        x = GS_MAX(x, 0);
        this->cursor = x;
    }

    void insert(char c)
    {
        GS_ASSERT(this->focused);
        size_t length_needed = this->text.length() + 1;
        if (length_needed > int(this->scratch_len)) return;

        strncpy(this->scratch, this->text.text, this->cursor);
        this->scratch[this->cursor] = c;
        strcpy(&this->scratch[this->cursor+1], &this->text.text[this->cursor]);

        this->text.set_text(this->scratch);
        this->set_cursor(this->cursor + 1);
    }

    void backspace()
    {
        GS_ASSERT(this->focused);
        if (cursor <= 0) return;
        this->text.text[this->cursor - 1] = '\0';
        this->set_cursor(this->cursor - 1);
    }

    void clear()
    {
        this->text.set_text("");
        this->cursor = 0;
    }

    void set_text(const char* text)
    {
        bool go_to_end = false;
        if (this->cursor == int(this->text.length()))
            go_to_end = true;
        this->text.set_text(text);
        if (go_to_end)
            this->cursor = int(this->text.length());
    }

    virtual void draw()
    {
        this->draw_background();
        this->draw_border();
        if (this->focused)
            this->draw_cursor();
        CHECK_GL_ERROR();
    }

    virtual void draw_text()
    {
        Vec2 p = vec2_add(this->position, this->border);
        p.y += this->text.get_height();
        this->text.set_position(p);
        if (this->password)
        {
            HudText::Text tmp = this->text;
            size_t len = tmp.length();
            for (size_t i=0; i<len; i++)
                tmp.text[i] = '*';
            tmp.draw();
        }
        else
        {
            this->text.draw();
        }
        this->draw_label();
        CHECK_GL_ERROR();
    }

    InputTextBox() :
        cursor(0), cursor_width(6),  cursor_color(Color(150, 150, 150)),
        password(false)
    {
        this->text.set_text("");
        this->text.set_color(Color(200, 200, 255));
    }

    virtual ~InputTextBox() {}

    private:
        const static size_t scratch_len = 0xFF;
        char scratch[scratch_len + 1];

    void draw_cursor()
    {
        int cursor_offset = 0;
        int cursor_height = this->dims.y - this->border.y;

        strncpy(this->scratch, this->text.text, this->cursor);
        this->scratch[this->cursor] = '\0';
        HudFont::font->get_string_pixel_dimension(this->scratch, &cursor_offset, NULL);
        Vec2 offset = this->position;
        offset.x += cursor_offset + this->border.x;
        offset.y += this->border.y * 0.5f;

        int cursor_width = this->cursor_width;
        if (this->text.text[0] != '\0' && HudFont::font != NULL)
        {
            HudFont::Glyph glyph = HudFont::font->get_glyph(this->text.text[0]);
            cursor_width = glyph.w;
        }
        draw_rect(this->cursor_color, offset, vec2_init(cursor_width, cursor_height));
    }
};

class Button: public InputBox
{
    public:

        bool activated;

    virtual void hover(const Vec2& cursor)
    {
        if (this->contains_point(cursor))
            this->focus();
        else
            this->unfocus();
    }

    virtual void click(const Vec2& cursor)
    {
        if (!this->contains_point(cursor)) return;
        this->activated = true;
    }

    bool was_activated()
    {
        bool a = this->activated;
        this->activated = false;
        return a;
    }

    const char* get_value()
    {
        return this->label.text;
    }

    virtual void draw()
    {
        this->draw_background();
        this->draw_border();
    }

    virtual void draw_label()
    {
        if (HudFont::font == NULL) return;

        this->label.set_text(this->get_name());
        if (this->focused)
            this->label.set_color(Color(220, 220, 220));
        else
            this->label.set_color(Color(128, 128, 128));

        int width = 0;
        int height = 0;
        HudFont::font->get_string_pixel_dimension(this->label.text, &width, &height);

        Vec2 p = this->position;
        p.y += height + this->border.y;
        p.x += this->dims.x / 2 - width / 2;
        this->label.set_position(p);
        this->label.draw();
        CHECK_GL_ERROR();
    }

    Button() :
        InputBox(), activated(false)
    {
        this->background_color.a = 0xFF;
    }

    virtual ~Button() {}

    protected:

    virtual void draw_background()
    {
        Vec2 b = vec2_scalar_mult(this->border, 0.5f);
        Vec2 p = vec2_add(this->position, b);
        Vec2 d = vec2_sub(this->dims, b);
        draw_rect(this->background_color, p, d);
    }

};

}   // Hud
