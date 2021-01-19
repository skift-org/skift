#include <libgraphic/Painter.h>

#include <libwidget/widgets/TextField.h>

TextField::TextField(Widget *parent, RefPtr<TextModel> model)
    : Widget(parent), _model(model)
{
}

TextField::~TextField()
{
}

void TextField::paint(Painter &painter, Recti rectangle)
{
    __unused(rectangle);

    painter.push();
    painter.transform(content_bound().position());

    int advance = 0;
    auto metrics = font()->metrics();
    int baseline = content_bound().height() / 2 + metrics.capheight() / 2;

    auto paint_cursor = [&](Painter &painter, int position) {
        Vec2 cursor_position{position, metrics.fullascend(baseline)};
        Vec2 cursor_size{2, metrics.fulllineheight()};
        Rect cursor_bound{cursor_position, cursor_size};

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

    painter.pop();
}

void TextField::scroll_to_cursor()
{
}

Vec2i TextField::size()
{
    return _model->line(0).bound(*font()).size();
}

void TextField::event(Event *event)
{
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
        if (event->keyboard.key == KEYBOARD_KEY_HOME)
        {
            _cursor.move_home_within(_model->line(_cursor.line()));
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_END)
        {
            _cursor.move_end_within(_model->line(_cursor.line()));
            scroll_to_cursor();
        }
        else if (event->keyboard.key == KEYBOARD_KEY_BKSPC)
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
            // ignore
        }
        else if (event->keyboard.codepoint != 0)
        {
            _model->append_at(_cursor, event->keyboard.codepoint);
            scroll_to_cursor();
        }

        should_repaint();

        event->accepted = true;
    }
}