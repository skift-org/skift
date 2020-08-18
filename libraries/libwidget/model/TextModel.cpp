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
    cursor.move_right_withing(*this);
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
        line(cursor.line()).backspace_at(cursor.column() - 1);
        cursor.move_left_withing(*this);
    }
}
