#include <libgraphic/Painter.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/TextField.h>

TextField::TextField(Widget *parent, RefPtr<TextModel> model)
    : Widget(parent),
      _model(model)
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

    if (_linenumbers)
    {
        painter.fill_rectangle(this->content_bound().take_left(32), color(THEME_BORDER));
        painter.fill_rectangle(this->content_bound().take_left(32).take_right(1), color(THEME_BORDER));
    }

    auto paint_cursor = [this](Painter &painter, Vec2i position) {
        if (!_readonly)
        {
            painter.draw_rectangle(Rectangle(position - Vec2(0, LINE_HEIGHT / 2 + 4), Vec2(2, LINE_HEIGHT)), color(THEME_ACCENT));
        }
    };

    for (size_t i = MAX(0, _vscroll_offset / LINE_HEIGHT - 1);
         i < MIN(_model->line_count(), (((unsigned)this->content_bound().height() + _vscroll_offset) / LINE_HEIGHT) + 1);
         i++)
    {
        Rectangle line_bound = document_bound().row(_model->line_count(), i);

        if (!_readonly)
        {
            if (_cursor.line() == i)
            {
                painter.fill_rectangle(line_bound.cutoff_left_and_right(32, 0), color(THEME_BORDER));
            }
        }

        if (_linenumbers)
        {
            // Line number
            char buffer[16];
            snprintf(buffer, 16, "%3d", (int)(i + 1));
            if (_cursor.line() == i)
            {
                painter.draw_string_within(*font(), buffer, line_bound.take_left(32).shrinked(Insets(0, 0, 0, 4)), Position::RIGHT, color(THEME_FOREGROUND));
            }
            else
            {
                painter.draw_string_within(*font(), buffer, line_bound.take_left(32).shrinked(Insets(0, 0, 0, 4)), Position::RIGHT, color(THEME_FOREGROUND).with_alpha(0.6));
            }
        }

        painter.push();

        if (_linenumbers)
        {
            painter.clip(content_bound().cutoff_left_and_right(32, 0));
        }

        // Line content
        auto &line = _model->line(i);

        Vec2i current_position = line_bound.cutoff_left_and_right((_linenumbers ? 32 : 0) + 4, 0).position() + Vec2i(0, LINE_HEIGHT / 2 + 4);

        for (size_t j = 0; j < line.length(); j++)
        {
            Codepoint codepoint = line[j];

            if (i == _cursor.line() && j == _cursor.column())
            {
                paint_cursor(painter, current_position);
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
                auto span = _model->span_at(i, j);

                auto glyph = font()->glyph(codepoint);
                painter.draw_glyph(*font(), glyph, current_position, color(span.foreground()));

                current_position += Vec2i(glyph.advance, 0);
            }
        }

        if (i == _cursor.line() && line.length() == _cursor.column())
        {
            paint_cursor(painter, current_position);
        }

        painter.pop();
    }
}

void TextField::event(Event *event)
{
    __unused(event);

    if (event->type == Event::KEYBOARD_KEY_TYPED)
    {
        if (event->keyboard.key == KEYBOARD_KEY_UP && event->keyboard.modifiers & KEY_MODIFIER_ALT)
        {
            _model->move_line_up_at(_cursor);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN && event->keyboard.modifiers & KEY_MODIFIER_ALT)
        {
            _model->move_line_down_at(_cursor);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_UP && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _vscroll_offset = MAX(_vscroll_offset - LINE_HEIGHT, 0);
            update_scrollbar();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _vscroll_offset = MIN(_vscroll_offset + LINE_HEIGHT, (int)_model->line_count() * LINE_HEIGHT - bound().height() + LINE_HEIGHT);
            update_scrollbar();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_PGUP && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _vscroll_offset = MAX(_vscroll_offset - bound().height() + LINE_HEIGHT, 0);
            update_scrollbar();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_PGDOWN && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _vscroll_offset = MIN(_vscroll_offset + bound().height() - LINE_HEIGHT, (int)_model->line_count() * LINE_HEIGHT - bound().height() + LINE_HEIGHT);
            update_scrollbar();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            _cursor.move_up_within(*_model);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN)
        {
            _cursor.move_down_within(*_model);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_LEFT)
        {
            _cursor.move_left_within(*_model);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_RIGHT)
        {
            _cursor.move_right_within(*_model);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_PGUP)
        {
            _cursor.move_up_within(*_model, bound().height() / LINE_HEIGHT);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_PGDOWN)
        {
            _cursor.move_down_within(*_model, bound().height() / LINE_HEIGHT);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_HOME && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _cursor.move_home_within(*_model);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_END && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _cursor.move_end_within(*_model);
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_HOME)
        {
            _cursor.move_home_within(_model->line(_cursor.line()));
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_END)
        {
            _cursor.move_end_within(_model->line(_cursor.line()));
            scroll_to_cursor();
        }

        if (!_readonly)
        {
            if (event->keyboard.key == KEYBOARD_KEY_BKSPC)
            {
                _model->backspace_at(_cursor);
                scroll_to_cursor();
            }
            else if (event->keyboard.key == KEYBOARD_KEY_DELETE)
            {
                _model->delete_at(_cursor);
                scroll_to_cursor();
            }
            else if (event->keyboard.key == KEYBOARD_KEY_ENTER)
            {
                _model->newline_at(_cursor);
                scroll_to_cursor();
            }
            else if (event->keyboard.codepoint != 0)
            {
                _model->append_at(_cursor, event->keyboard.codepoint);
                scroll_to_cursor();
            }
        }

        should_repaint();

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

void TextField::update_scrollbar()
{
    int document_height = document_bound().height();

    if (_overscroll)
    {
        document_height += view_bound().height() - LINE_HEIGHT;
    }

    _vscrollbar->update(
        document_height,
        view_bound().height(),
        _vscroll_offset);

    _hscrollbar->update(
        document_bound().width() + ScrollBar::SIZE,
        view_bound().width() - 32,
        _hscroll_offset);
}

void TextField::do_layout()
{
    _vscrollbar->bound(vscrollbar_bound());
    _hscrollbar->bound(hscrollbar_bound());

    update_scrollbar();
}

void TextField::scroll_to_cursor()
{
    if ((int)_cursor.line() * LINE_HEIGHT < _vscroll_offset)
    {
        _vscroll_offset = _cursor.line() * LINE_HEIGHT;
    }

    if ((int)(_cursor.line()) * LINE_HEIGHT > _vscroll_offset + content_bound().height() - LINE_HEIGHT - LINE_HEIGHT)
    {
        _vscroll_offset = (_cursor.line() + 2) * LINE_HEIGHT - content_bound().height();
    }

    update_scrollbar();
}
