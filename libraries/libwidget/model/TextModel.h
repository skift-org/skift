#pragma once

#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/Lexer.h>

#include <libutils/OwnPtr.h>
#include <libutils/RefCounted.h>
#include <libutils/Vector.h>

struct TextCursor;

class TextModelLine
{
private:
    Vector<Codepoint> _codepoints{};

public:
    TextModelLine()
    {
    }

    ~TextModelLine()
    {
    }

    Codepoint operator[](size_t index)
    {
        assert(index < length());

        return _codepoints[index];
    }

    size_t length()
    {
        return _codepoints.count();
    }

    void append(Codepoint codepoint)
    {
        _codepoints.push_back(codepoint);
    }

    void append_at(size_t index, Codepoint codepoint)
    {
        _codepoints.insert(index, codepoint);
    }

    void backspace_at(size_t index)
    {
        _codepoints.remove_index(index);
    }
};

class TextModel : public RefCounted<TextModel>
{
private:
    Vector<OwnPtr<TextModelLine>> _lines{1024};

public:
    static RefPtr<TextModel> empty();

    static RefPtr<TextModel> from_file(const char *path);

    TextModel() {}

    ~TextModel() {}

    TextModelLine &line(int index) { return *_lines[index]; }

    size_t line_count() { return _lines.count(); }

    void append_line(OwnPtr<TextModelLine> line) { _lines.push_back(line); }

    void append_at(TextCursor &cursor, Codepoint codepoint);

    void backspace_at(TextCursor &cursor);
};

struct TextCursor
{
private:
    size_t _line = 0;
    size_t _column = 0;
    size_t _prefered_column = 0;

public:
    size_t line() { return _line; }
    size_t column() { return _column; }

    void move_up_within(TextModel &model)
    {
        __unused(model);

        if (_line > 0)
        {
            _line--;
            _column = clamp(_prefered_column, 0, model.line(_line).length() - 1);
        }
    }

    void move_down_within(TextModel &model)
    {
        if (_line < model.line_count() - 1)
        {
            _line++;
            _column = clamp(_prefered_column, 0, model.line(_line).length() - 1);
        }
    }

    void move_to_within(TextModel &model, size_t line)
    {
        if (line < model.line_count())
        {
            _line = line;
        }
    }

    void move_left_withing(TextModel &model)
    {
        __unused(model);

        if (_column > 0)
        {
            _column--;
        }
        else if (_column == 0 && _line > 0)
        {
            move_up_within(model);
            _column = model.line(_line).length() - 1;
        }

        _prefered_column = _column;
    }

    void move_right_withing(TextModel &model)
    {
        if (_column < model.line(_line).length())
        {
            _column++;
        }
        else if (_column == model.line(_line).length() &&
                 _line < model.line_count() - 1)
        {
            move_down_within(model);
            _column = 0;
        }

        _prefered_column = _column;
    }
};
