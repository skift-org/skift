#include <libwidget/model/TextModel.h>

RefPtr<TextModel> TextModel::empty()
{
    auto model = make<TextModel>();
    model->append_line(own<TextModelLine>());
    return model;
}

RefPtr<TextModel> TextModel::from_file(const char *path)
{
    auto model = make<TextModel>();

    Stream *stream = stream_open(path, OPEN_READ | OPEN_BUFFERED);

    Lexer lexer(stream);

    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");

    while (lexer.do_continue())
    {
        auto line = own<TextModelLine>();

        while (lexer.do_continue() && lexer.current_codepoint() != '\n')
        {
            line->append(lexer.current_codepoint());

            lexer.foreward_codepoint();
        }

        model->append_line(line);
        lexer.foreward_codepoint();
    }

    stream_close(stream);

    if (model->line_count() == 0)
    {
        model->append_line(own<TextModelLine>());
    }

    return model;
}

void TextModel::append_at(TextCursor &cursor, Codepoint codepoint)
{
    line(cursor.line()).append_at(cursor.column(), codepoint);
    cursor.move_right_within(*this);
}

void TextModel::backspace_at(TextCursor &cursor)
{
    if (cursor.line() > 0 &&
        cursor.column() == 0)
    {
        int line_length = line(cursor.line() - 1).length();
        line(cursor.line() - 1).append(line(cursor.line()));

        _lines.remove_index(cursor.line());

        cursor.move_up_within(*this);
        cursor.move_to_within(line(cursor.line()), line_length);
    }
    else if (cursor.column() > 0 && line(cursor.line()).length() > 0)
    {
        line(cursor.line()).backspace_at(cursor.column());
        cursor.move_left_within(*this);
    }
}

void TextModel::delete_at(TextCursor &cursor)
{
    if (cursor.line() < line_count() - 1 && cursor.column() == line(cursor.line()).length())
    {
        line(cursor.line()).append(line(cursor.line() + 1));

        _lines.remove_index(cursor.line() + 1);
    }
    else if (cursor.column() < line(cursor.line()).length() && line(cursor.line()).length() > 0)
    {
        line(cursor.line()).delete_at(cursor.column());
    }
}

void TextModel::newline_at(TextCursor &cursor)
{
    auto left_side_line = own<TextModelLine>();
    auto right_side_line = own<TextModelLine>();

    for (size_t i = 0; i < cursor.column(); i++)
    {
        left_side_line->append(line(cursor.line())[i]);
    }

    for (size_t i = cursor.column(); i < line(cursor.line()).length(); i++)
    {
        right_side_line->append(line(cursor.line())[i]);
    }

    _lines.remove_index(cursor.line());

    _lines.insert(cursor.line(), left_side_line);
    _lines.insert(cursor.line() + 1, right_side_line);

    cursor.move_down_within(*this);
    cursor.move_to_within(*right_side_line, 0);
}

void TextModel::move_line_up_at(TextCursor &cursor)
{
    if (cursor.line() > 0)
    {
        _lines.insert(cursor.line() - 1, _lines.take_at(cursor.line()));
        cursor.move_up_within(*this);
    }
}

void TextModel::move_line_down_at(TextCursor &cursor)
{
    if (cursor.line() < line_count())
    {
        _lines.insert(cursor.line() + 1, _lines.take_at(cursor.line()));
        cursor.move_down_within(*this);
    }
}
