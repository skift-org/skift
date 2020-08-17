#include <libgraphic/Painter.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/TextField.h>

TextField::TextField(Widget *parent, RefPtr<TextModel> model)
    : Widget(parent), _model(model)
{
    _vscrollbar = new ScrollBar(this);
    _vscrollbar->on(Event::VALUE_CHANGE, [this](auto) {
        _vscroll_offset = _vscrollbar->value();
        should_repaint();
    });

    _hscrollbar = new ScrollBar(this);
    _hscrollbar->horizontal(true);

    _hscrollbar->on(Event::VALUE_CHANGE, [this](auto) {
        _hscroll_offset = _hscrollbar->value();
        should_repaint();
    });
}

TextField::~TextField()
{
}

#define LINE_HEIGHT (18)

void TextField::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.fill_rectangle(this->content_bound().take_left(32), color(THEME_BORDER));
    painter.fill_rectangle(this->content_bound().take_left(32).take_right(1), color(THEME_BORDER));

    for (size_t i = MAX(0, _vscroll_offset / LINE_HEIGHT - 1);
         i < MIN(_model->line_count(), (((unsigned)this->content_bound().height() + _vscroll_offset) / LINE_HEIGHT) + 1);
         i++)
    {
        Rectangle line_bound = Rectangle(content_bound().position() + Vec2i(0, LINE_HEIGHT * i - _vscroll_offset), Vec2i(content_bound().width(), LINE_HEIGHT));

        if (_cursor.line() == i)
        {
            painter.fill_rectangle(line_bound.cutoff_left_and_right(32, 0), color(THEME_BORDER));
        }

        // Line number
        char buffer[16];
        snprintf(buffer, 16, "%3d", (int)(i + 1));
        if (_cursor.line() == i)
        {
            painter.draw_string_within(*font(), buffer, line_bound.take_left(32).shrinked(Insets(0, 0, 0, 4)), Position::RIGHT, color(THEME_FOREGROUND));
        }
        else
        {
            painter.draw_string_within(*font(), buffer, line_bound.take_left(32).shrinked(Insets(0, 0, 0, 4)), Position::RIGHT, ALPHA(color(THEME_FOREGROUND), 0.6));
        }

        painter.push();
        painter.clip(content_bound().cutoff_left_and_right(32, 0));
        // Line content
        auto line = _model->line(i);

        Vec2i current_position = line_bound.cutoff_left_and_right(32 + 4, 0).position() + Vec2i(-_hscroll_offset, LINE_HEIGHT / 2 + 4);

        for (size_t j = 0; j < line.length(); j++)
        {
            Codepoint codepoint = line[j];

            if (i == _cursor.line() && j == _cursor.column())
            {
                painter.draw_rectangle(Rectangle(current_position - Vec2(0, LINE_HEIGHT / 2 + 4), Vec2(2, LINE_HEIGHT)), color(THEME_ACCENT));
            }

            if (codepoint == U'\t')
            {
                current_position += Vec2i(8 * 4, 0);
            }
            else if (codepoint == U'\r')
            {
                // ignore
            }
            else
            {
                auto glyph = font()->glyph(codepoint);
                painter.draw_glyph(*font(), glyph, current_position, COLOR_WHITE);

                current_position += Vec2i(glyph.advance, 0);
            }
        }

        if (i == _cursor.line() && line.length() == _cursor.column())
        {
            painter.draw_rectangle(Rectangle(current_position - Vec2(0, LINE_HEIGHT / 2 + 4), Vec2(2, LINE_HEIGHT)), color(THEME_ACCENT));
        }

        painter.pop();
    }
}

void TextField::event(Event *event)
{
    __unused(event);

    if (event->type == Event::KEYBOARD_KEY_TYPED)
    {
        if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            _cursor.move_up_within(*_model);
            should_repaint();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN)
        {
            _cursor.move_down_within(*_model);
            should_repaint();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_LEFT)
        {
            _cursor.move_left_withing(*_model);
            should_repaint();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_RIGHT)
        {
            _cursor.move_right_withing(*_model);
            should_repaint();
        }
        else if (event->keyboard.codepoint != 0)
        {
            _model->append_at(_cursor, event->keyboard.codepoint);
            _cursor.move_right_withing(*_model);

            should_repaint();
        }

        event->accepted = true;
    }
    else if (event->type == Event::MOUSE_BUTTON_PRESS)
    {
        size_t line = ((size_t)(event->mouse.position - content_bound().position()).y() + _vscroll_offset) / LINE_HEIGHT;

        _cursor.move_to_within(*_model, line);
        should_repaint();
        event->accepted = true;
    }
}

void TextField::do_layout()
{
    _vscrollbar->bound(this->content_bound().take_right(16).cutoff_top_and_botton(0, 16));
    _vscrollbar->update((_model->line_count() + 1) * LINE_HEIGHT, content_bound().height());

    _hscrollbar->bound(this->content_bound().take_bottom(16).cutoff_left_and_right(32, 16));
    _hscrollbar->update(1000, content_bound().width() - 32);
}
