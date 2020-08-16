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
};

class TextModel : public RefCounted<TextModel>
{
private:
    Vector<OwnPtr<TextModelLine>> _lines{1024};

public:
    static RefPtr<TextModel> from_file(const char *path)
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

    TextModel()
    {
    }

    ~TextModel() {}

    TextModelLine &line(int index) { return *_lines[index]; }

    size_t line_count() { return _lines.count(); }

    void append_line(OwnPtr<TextModelLine> line) { _lines.push_back(line); }
};

struct TextCursor
{
private:
    size_t _line = 0;
    size_t _column = 0;

public:
    size_t line() { return _line; }
    size_t column() { return _column; }

    void move_up_within(TextModel &model)
    {
        __unused(model);
        if (_line > 0)
        {
            _line--;
        }
    }

    void move_down_within(TextModel &model)
    {
        if (_line < model.line_count() - 1)
        {
            _line++;
        }
    }

    void move_to_within(TextModel &model, size_t line)
    {
        if (line < model.line_count())
        {
            _line = line;
        }
    }
};
