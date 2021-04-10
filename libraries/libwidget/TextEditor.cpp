#include <libgraphic/Painter.h>
#include <libwidget/TextEditor.h>
#include <libwidget/Window.h>

namespace Widget
{

TextEditor::TextEditor(Component *parent, RefPtr<TextModel> model)
    : Component(parent),
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

TextEditor::~TextEditor()
{
}

void TextEditor::paint(Graphic::Painter &painter, const Math::Recti &)
{
    auto metrics = font()->metrics();

    size_t first_visible_line = MAX(0, _vscroll_offset / metrics.fulllineheight() - 1);
    size_t last_visible_line = MIN((int)_model->line_count(), (bound().height() + _vscroll_offset) / metrics.fulllineheight() + 1);

    painter.push();
    painter.transform(Math::Vec2i{0, -_vscroll_offset} + view_bound().position().extract_y());

    for (size_t i = first_visible_line; i < last_visible_line; i++)
    {
        auto &line = _model->line(i);
        int linetop = metrics.fulllineheight() * i;
        int baseline = linetop + metrics.halfleading() + metrics.baseline();

        //painter.draw_line({0, baseline}, {bound().width(), baseline}, Colors::RED);

        if (!_readonly && _cursor.line() == i)
        {
            painter.fill_rectangle({0, linetop, bound().width(), metrics.fulllineheight()}, color(THEME_BORDER));
        }

        painter.push();
        painter.transform(Math::Vec2i{-_hscroll_offset, 0} + view_bound().position().extract_x());

        auto paint_cursor = [&](Graphic::Painter &painter, int position) {
            Math::Vec2i cursor_position{position, metrics.fullascend(baseline)};
            Math::Vec2i cursor_size{2, metrics.fulllineheight()};
            Math::Recti cursor_bound{cursor_position, cursor_size};

            painter.fill_rectangle(cursor_bound, color(THEME_ACCENT));
        };

        int advance = 0;
        for (size_t j = 0; j < line.length(); j++)
        {
            Codepoint codepoint = line[j];

            if (i == _cursor.line() && j == _cursor.column())
            {
                paint_cursor(painter, advance);
            }

            if (codepoint == U'\t')
            {
                advance += font()->mesure(U' ').width();
            }
            else if (codepoint == U'\r')
            {
                // ignore
            }
            else
            {
                auto span = _model->span_at(i, j);

                auto glyph = font()->glyph(codepoint);
                painter.draw_glyph(*font(), glyph, {advance, baseline}, color(span.foreground()));
                advance += glyph.advance;
            }
        }

        if (i == _cursor.line() && line.length() == _cursor.column())
        {
            paint_cursor(painter, advance);
        }

        painter.pop();
    }

    painter.pop();
}

void TextEditor::event(Event *event)
{
    auto metrics = font()->metrics();

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
            _vscroll_offset = MAX(_vscroll_offset - metrics.fulllineheight(), 0);
            update_scrollbar();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _vscroll_offset = MIN(_vscroll_offset + metrics.fulllineheight(), (int)_model->line_count() * metrics.fulllineheight() - bound().height() + metrics.fulllineheight());
            update_scrollbar();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_PGUP && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _vscroll_offset = MAX(_vscroll_offset - bound().height() + metrics.fulllineheight(), 0);
            update_scrollbar();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_PGDOWN && event->keyboard.modifiers & KEY_MODIFIER_CTRL)
        {
            _vscroll_offset = MIN(_vscroll_offset + bound().height() - metrics.fulllineheight(), (int)_model->line_count() * metrics.fulllineheight() - bound().height() + metrics.fulllineheight());
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
            _cursor.move_up_within(*_model, bound().height() / metrics.fulllineheight());
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_PGDOWN)
        {
            _cursor.move_down_within(*_model, bound().height() / metrics.fulllineheight());
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
    else if (event->type == Event::MOUSE_SCROLL)
    {
        event->accepted = true;
        _vscrollbar->dispatch_event(event);
    }
    else if (event->type == Event::MOUSE_BUTTON_PRESS)
    {
        size_t line = ((size_t)(event->mouse.position - bound().position()).y() + _vscroll_offset) / metrics.fulllineheight();

        _cursor.move_to_within(*_model, line);
        should_repaint();
        event->accepted = true;
    }
}

void TextEditor::update_scrollbar()
{
    auto metrics = font()->metrics();

    int document_height = document_bound().height();

    if (_overscroll)
    {
        document_height += view_bound().height() - metrics.fulllineheight();
    }

    _vscrollbar->update(
        document_height,
        view_bound().height(),
        _vscroll_offset);

    _hscrollbar->update(
        document_bound().width(),
        view_bound().width(),
        _hscroll_offset);
}

void TextEditor::do_layout()
{
    _vscrollbar->container(vscrollbar_bound());
    _hscrollbar->container(hscrollbar_bound());

    update_scrollbar();
}

void TextEditor::scroll_to_cursor()
{
    auto metrics = font()->metrics();

    if ((int)_cursor.line() * metrics.fulllineheight() < _vscroll_offset)
    {
        _vscroll_offset = _cursor.line() * metrics.fulllineheight();
    }

    if ((int)(_cursor.line()) * metrics.fulllineheight() > _vscroll_offset + bound().height() - metrics.fulllineheight() - metrics.fulllineheight())
    {
        _vscroll_offset = (_cursor.line() + 2) * metrics.fulllineheight() - bound().height();
    }

    update_scrollbar();
}

} // namespace Widget
