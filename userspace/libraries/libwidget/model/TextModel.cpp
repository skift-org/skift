#include "libio/MemoryReader.h"
#include <libio/BufReader.h>
#include <libio/File.h>
#include <libio/Scanner.h>
#include <libwidget/model/TextModel.h>

namespace Widget
{

RefPtr<TextModel> TextModel::empty()
{
    auto model = make<TextModel>();
    model->append_line(own<TextModelLine>());
    return model;
}

RefPtr<TextModel> TextModel::open(String path)
{
    auto model = make<TextModel>();

    IO::File file{path, HJ_OPEN_READ};
    IO::BufReader buf_reader{file, 512};
    IO::Scanner scan{buf_reader};

    // Skip the utf8 bom header if present.
    scan.skip_word("\xEF\xBB\xBF");

    while (!scan.ended())
    {
        auto line = own<TextModelLine>();

        while (!scan.ended() &&
               scan.peek_rune() != '\n')
        {
            line->append(scan.peek_rune());
            scan.next_rune();
        }

        scan.skip('\n'); // skip the \n

        model->append_line(line);
    }

    if (model->line_count() == 0)
    {
        model->append_line(own<TextModelLine>());
    }

    if (model->line_count() == 0)
    {
        model->append_line(own<TextModelLine>());
    }

    return model;
}

RefPtr<TextModel> TextModel::create(String text)
{
    auto model = make<TextModel>();

    IO::MemoryReader memory{text};
    IO::Scanner scan{memory};

    // Skip the utf8 bom header if present.
    scan.skip_word("\xEF\xBB\xBF");

    while (!scan.ended())
    {
        auto line = own<TextModelLine>();

        while (!scan.ended() &&
               scan.peek_rune() != '\n')
        {
            line->append(scan.peek_rune());
            scan.next_rune();
        }

        scan.skip('\n'); // skip the \n

        model->append_line(line);
    }

    if (model->line_count() == 0)
    {
        model->append_line(own<TextModelLine>());
    }

    if (model->line_count() == 0)
    {
        model->append_line(own<TextModelLine>());
    }

    return model;
}

String TextModel::string()
{
    IO::MemoryWriter memory;

    for (size_t i = 0; i < _lines.count(); i++)
    {
        for (size_t j = 0; j < _lines[i]->length(); j++)
        {
            char buffer[5];
            Text::rune_to_utf8(_lines[i]->operator[](j), (uint8_t *)buffer);
            IO::write(memory, buffer);
        }

        if (i + 1 < _lines.count())
        {
            IO::write(memory, '\n');
        }
    }

    return memory.string();
}

ResultOr<size_t> TextModel::save(String path)
{
    IO::File file{path, HJ_OPEN_WRITE | HJ_OPEN_CREATE};
    return IO::write(file, string());
}

void TextModel::append_at(TextCursor &cursor, Text::Rune rune)
{
    line(cursor.line()).append_at(cursor.column(), rune);
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
