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
}
