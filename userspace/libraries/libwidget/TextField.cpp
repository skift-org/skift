#include <libgraphic/Painter.h>
#include <libwidget/TextField.h>

namespace Widget
{

TextField::TextField(RefPtr<TextModel> model)
    : _model(model)
{
    _model_observer = _model->observe([this](auto &) {
        _cursor.clamp_within(*_model);

        scroll_to_cursor();
        should_repaint();
    });
}

TextField::~TextField()
{
}

void TextField::paint(Graphic::Painter &painter, const Math::Recti &)
{
    int advance = 0;
    auto metrics = font()->metrics();
    int baseline = bound().height() / 2 + metrics.capheight() / 2;

    auto paint_cursor = [&](Graphic::Painter &painter, int position) {
        Math::Vec2i cursor_position{position, metrics.fullascend(baseline)};
        Math::Vec2i cursor_size{2, metrics.fulllineheight()};
        Math::Recti cursor_bound{cursor_position, cursor_size};

        painter.fill_rectangle(cursor_bound, color(THEME_ACCENT));
    };

    auto &line = _model->line(0);

    for (size_t j = 0; j < line.length(); j++)
    {
        Codepoint codepoint = line[j];

        if (j == _cursor.column())
        {
            paint_cursor(painter, advance);
        }

        if (codepoint == U'\t')
        {
            advance += 8 * 4;
        }
        else if (codepoint == U'\r')
        {
            // ignore
        }
        else
        {
            auto span = _model->span_at(0, j);

            auto glyph = font()->glyph(codepoint);
            painter.draw_glyph(*font(), glyph, {advance, baseline}, color(span.foreground()));

            advance += glyph.advance;
        }
    }

    if (line.length() == _cursor.column())
    {
        paint_cursor(painter, advance);
    }
}

void TextField::scroll_to_cursor()
{
}

Math::Vec2i TextField::size()
{
    return _model->line(0).bound(*font()).size();
}

String TextField::text()
{
    return _model->string();
}

void TextField::event(Event *event)
{
    if (event->type == Event::KEYBOARD_KEY_TYPED)
    {
        if (event->keyboard.key == KEYBOARD_KEY_UP && event->keyboard.modifiers & KEY_MODIFIER_ALT)
        {
            _model->move_line_up_at(_cursor);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN && event->keyboard.modifiers & KEY_MODIFIER_ALT)
        {
            _model->move_line_down_at(_cursor);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            _cursor.move_up_within(*_model);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN)
        {
            _cursor.move_down_within(*_model);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_LEFT)
        {
            _cursor.move_left_within(*_model);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_RIGHT)
        {
            _cursor.move_right_within(*_model);
        }
        if (event->keyboard.key == KEYBOARD_KEY_HOME)
        {
            _cursor.move_home_within(_model->line(_cursor.line()));
        }
        else if (event->keyboard.key == KEYBOARD_KEY_END)
        {
            _cursor.move_end_within(_model->line(_cursor.line()));
        }
        else if (event->keyboard.key == KEYBOARD_KEY_BKSPC)
        {
            _model->backspace_at(_cursor);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DELETE)
        {
            _model->delete_at(_cursor);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_ENTER)
        {
            // ignore
        }
        else if (event->keyboard.codepoint != 0)
        {
            _model->append_at(_cursor, event->keyboard.codepoint);
        }

        event->accepted = true;
    }
}

} // namespace Widget
