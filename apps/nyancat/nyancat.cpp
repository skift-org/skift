/*
 * Copyright (c) 2011-2018 K. Lange.  All rights reserved.
 *
 * Developed by:            K. Lange
 *                          http://github.com/klange/nyancat
 *                          http://nyancat.dakko.us
 */

#include <libio/Streams.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/System.h>

#include "nyancat/animation.h"

/*
 * Color palette to use for final output
 * Specifically, this should be either control sequences
 * or raw characters (ie, for vt220 mode)
 */
const char *colors[256] = {nullptr};

/*
 * For most modes, we output spaces, but for some
 * we will use block characters (or even nothing)
 */
const char *output = "  ";

/*
 * Clear the screen between frames (as opposed to resetting
 * the cursor position)
 */
int clear_screen = 1;

/*
 * Force-set the terminal title.
 */
int set_title = 1;

/*
 * I refuse to include libm to keep this low
 * on external dependencies.
 *
 * Count the number of digits in a number for
 * use with string output.
 */
int digits(int val)
{
    int d = 1, c;
    if (val >= 0)
        for (c = 10; c <= val; c *= 10)
            d++;
    else
        for (c = -10; c >= val; c *= 10)
            d++;
    return (c < 0) ? ++d : d;
}

/*
 * These values crop the animation, as we have a full 64x64 stored,
 * but we only want to display 40x24 (double width).
 */
int min_row = -1;
int max_row = -1;
int min_col = -1;
int max_col = -1;

/*
 * Actual width/height of terminal.
 */
int terminal_width = 80;
int terminal_height = 24;

/*
 * Flags to keep track of whether width/height were automatically set.
 */
char using_automatic_width = 0;
char using_automatic_height = 0;

/*
 * Print escape sequences to return cursor to visible mode
 * and exit the application.
 */
void finish()
{
    IO::out("\033[0m\n");
    process_exit(PROCESS_SUCCESS);
}

void newline(int n)
{
    for (int i = 0; i < n; ++i)
    {
        IO::out("\n");
    }
}

void update_terminal_size()
{
    IOCallTerminalSizeArgs terminal_size;
    stream_call(out_stream, IOCALL_TERMINAL_GET_SIZE, &terminal_size);

    if (handle_has_error(out_stream))
    {
        return;
    }

    terminal_width = terminal_size.width;
    terminal_height = terminal_size.height;

    min_col = (FRAME_WIDTH - terminal_width / 2) / 2;
    max_col = (FRAME_WIDTH + terminal_width / 2) / 2;

    min_row = (FRAME_HEIGHT - (terminal_height - 1)) / 2;
    max_row = (FRAME_HEIGHT + (terminal_height - 1)) / 2;
}

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    colors[','] = "\033[104m"; /* Blue background */
    colors['.'] = "\033[107m"; /* White stars */
    colors['\''] = "\033[40m"; /* Black border */
    colors['@'] = "\033[47m";  /* Tan poptart */
    colors['$'] = "\033[105m"; /* Pink poptart */
    colors['-'] = "\033[101m"; /* Red poptart */
    colors['>'] = "\033[101m"; /* Red rainbow */
    colors['&'] = "\033[43m";  /* Orange rainbow */
    colors['+'] = "\033[103m"; /* Yellow Rainbow */
    colors['#'] = "\033[102m"; /* Green rainbow */
    colors['='] = "\033[104m"; /* Light blue rainbow */
    colors[';'] = "\033[44m";  /* Dark blue rainbow */
    colors['*'] = "\033[100m"; /* Gray cat face */
    colors['%'] = "\033[105m"; /* Pink cheeks */

    IO::out("\033[H\033[2J");

    size_t i = 0;  /* Current frame # */
    char last = 0; /* Last color index rendered */
    int y, x;      /* x/y coordinates of what we're drawing */

    auto start_time = system_get_ticks();

    while (true)
    {
        update_terminal_size();

        if (handle_has_error(out_stream))
        {
            return PROCESS_FAILURE;
        }

        /* Reset cursor */
        IO::out("\033[H");

        /* Render the frame */
        for (y = min_row; y < max_row; ++y)
        {
            for (x = min_col; x < max_col; ++x)
            {
                char color;
                if (y > 23 && y < 43 && x < 0)
                {
                    /*
					 * Generate the rainbow tail.
					 *
					 * This is done with a pretty simplistic square wave.
					 */
                    int mod_x = ((-x + 2) % 16) / 8;
                    if ((i / 2) % 2)
                    {
                        mod_x = 1 - mod_x;
                    }

                    // Our rainbow, with some padding.
                    const char *rainbow = ",,>>&&&+++###==;;;,,";

                    color = rainbow[mod_x + y - 23];

                    if (color == 0)
                        color = ',';
                }
                else if (x < 0 || y < 0 || y >= FRAME_HEIGHT || x >= FRAME_WIDTH)
                {
                    /* Fill all other areas with background */
                    color = ',';
                }
                else
                {
                    /* Otherwise, get the color from the animation frame. */
                    color = frames[i][y][x];
                }

                if (color != last && colors[(int)color])
                {
                    /* Normal Mode, send escape (because the color changed) */
                    last = color;
                    IO::out("{}{}", colors[(int)color], output);
                }
                else
                {
                    /* Same color, just send the output characters */
                    IO::out(output);
                }
            }

            newline(1);
        }

        int width = (terminal_width - 29 - 5) / 2;

        while (width > 0)
        {
            IO::out(" ");
            width--;
        }

        IO::out("\033[1;37mYou have nyaned for {} seconds!\033[J\033[0m", (system_get_ticks() - start_time) / 1000);

        last = 0;

        ++i;

        if (!frames[i])
        {
            i = 0;
        }

        process_sleep(90);
    }
}
