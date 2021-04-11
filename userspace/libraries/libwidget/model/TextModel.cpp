#include <libutils/Scanner.h>
#include <libutils/StringBuilder.h>
#include <libwidget/model/TextModel.h>

namespace Widget
{

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

    StreamScanner scan{stream};

    // Skip the utf8 bom header if present.
    scan.skip_word("\xEF\xBB\xBF");

    while (scan.do_continue())
    {
        auto line = own<TextModelLine>();

        while (scan.do_continue() &&
               scan.current_codepoint() != '\n')
        {
            line->append(scan.current_codepoint());
            scan.foreward_codepoint();
        }

        scan.skip('\n'); // skip the \n

        model->append_line(line);
    }

    stream_close(stream);

    if (model->line_count() == 0)
    {
        model->append_line(own<TextModelLine>());
    }

    model->span_add(TextModelSpan(0, 0, 10, THEME_ANSI_RED, THEME_ANSI_BLUE));

    if (model->line_count() == 0)
    {
        model->append_line(own<TextModelLine>());
    }

    return model;
}

String TextModel::string()
{
    StringBuilder builder;

    for (size_t i = 0; i < _lines.count(); i++)
    {
        for (size_t j = 0; j < _lines[i]->length(); j++)
        {
            builder.append_codepoint(_lines[i]->operator[](j));
        }

        if (i + 1 < _lines.count())
        {
            builder.append('\n');
        }
    }

    return builder.finalize();
}

void TextModel::append_at(TextCursor &cursor, Codepoint codepoint)
{
    line(cursor.line()).append_at(cursor.column(), codepoint);
    cursor.move_right_within(*this);
    did_update();
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

        did_update();
    }
    else if (cursor.column() > 0 && line(cursor.line()).length() > 0)
    {
        line(cursor.line()).backspace_at(cursor.column());
        cursor.move_left_within(*this);

        did_update();
    }
}

void TextModel::delete_at(TextCursor &cursor)
{
    if (cursor.line() < line_count() - 1 && cursor.column() == line(cursor.line()).length())
    {
        line(cursor.line()).append(line(cursor.line() + 1));

        _lines.remove_index(cursor.line() + 1);

        did_update();
    }
    else if (cursor.column() < line(cursor.line()).length() && line(cursor.line()).length() > 0)
    {
        line(cursor.line()).delete_at(cursor.column());

        did_update();
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
    cursor.move_to_beginning_of_the_line();

    did_update();
}

void TextModel::move_line_up_at(TextCursor &cursor)
{
    if (cursor.line() > 0)
    {
        _lines.insert(cursor.line() - 1, _lines.take_at(cursor.line()));
        cursor.move_up_within(*this);

        did_update();
    }
}

void TextModel::move_line_down_at(TextCursor &cursor)
{
    if (cursor.line() < line_count())
    {
        _lines.insert(cursor.line() + 1, _lines.take_at(cursor.line()));
        cursor.move_down_within(*this);

        did_update();
    }
}

} // namespace Widget
