#pragma once

#include <libio/Format.h>
#include <libio/Reader.h>
#include <libio/Scanner.h>
#include <libio/Writer.h>
#include <libshell/History.h>
#include <libshell/Line.h>
#include <libshell/LineEditor.h>

namespace Shell
{

ResultOr<String> readline(
    IO::Reader &input,
    IO::Writer &output,
    History &history)
{
    size_t last_cursor = 0;
    Optional<size_t> history_index;
    LineEditor editor{};
    IO::Scanner scan{input};
    bool should_continue = true;

    auto recall_history = [&]() {
        if (history_index.present())
        {
            editor.line(history.peek(history_index.unwrap()));
            history_index.clear();
        }
    };

    auto current_line = [&]() {
        if (history_index.present())
        {
            return history.peek(history_index.unwrap());
        }
        else
        {
            return editor.line();
        }
    };

    auto current_cursor = [&]() {
        if (history_index.present())
        {
            return history.peek(history_index.unwrap()).count();
        }
        else
        {
            return editor.cursor();
        }
    };

    auto line_as_string = [&]() -> String {
        IO::MemoryWriter memory;

        for (auto &rune : current_line())
        {
            char buffer[5];
            Text::rune_to_utf8(rune, (uint8_t *)buffer);
            IO::write(memory, buffer);
        }

        return memory.string();
    };

    auto render = [&]() {
        TRY(IO::format(output, "\e[{}D\e[J", last_cursor));
        TRY(IO::write(output, line_as_string()));
        TRY(IO::format(output, "\e[{}D", current_line().count()));
        TRY(IO::format(output, "\e[{}C", current_cursor()));

        last_cursor = current_cursor();

        return SUCCESS;
    };

    while (should_continue && !scan.ended())
    {
        if (scan.skip('\r') || scan.skip('\n'))
        {
            recall_history();
            should_continue = false;
        }
        else if (scan.skip('\b') || scan.skip(127))
        {
            recall_history();
            editor.backspace();
        }
        else if (scan.skip('\t'))
        {
            // ignore
        }
        else if (scan.skip_word("\e[A"))
        {
            if (!history_index.present())
            {
                history.commit(current_line());
                history_index = 0;
            }
            else if (history_index.unwrap_or(0) + 1 < history.length())
            {
                history_index = history_index.unwrap_or(0) + 1;
            }
        }
        else if (scan.skip_word("\e[B"))
        {
            if (history_index.unwrap_or(0) > 0)
            {
                history_index = history_index.unwrap_or(0) - 1;
            }
        }
        else if (scan.skip_word("\e[C"))
        {
            recall_history();
            editor.move_right();
        }
        else if (scan.skip_word("\e[D"))
        {
            recall_history();
            editor.move_left();
        }
        else if (scan.skip_word("\e[1~"))
        {
            recall_history();
            editor.move_home();
        }
        else if (scan.skip_word("\e[3~"))
        {
            recall_history();
            editor.delete_();
        }
        else if (scan.skip_word("\e[4~"))
        {
            recall_history();
            editor.move_end();
        }
        else
        {
            recall_history();
            Text::Rune rune = scan.peek_rune();
            scan.next_rune();
            editor.insert(rune);
        }

        TRY(render());
    }

    TRY(IO::write(output, '\n'));
    history.commit(current_line());
    return String{line_as_string()};
}

} // namespace Shell
