#include <stdlib.h>

#include "kernel/console.h"

#include "devices/uart.h"
#include "devices/vga.h"
#include "utils/ascii.h"
#include "sync/lock.h"

#define TAB_SIZE 4

console_t console;
lock_t console_lock;
bool console_bypass_lock = false;

void console_setup()
{
    console.cursor.x = 0;
    console.cursor.y = 0;
    console.bg = console_defaul_bg;
    console.fg = console_defaul_fg;

    lock_init(&console_lock, "console");

    vga_setup();
    uart_setup();
}

void console_color(console_color_t fg, console_color_t bg)
{
    console.fg = fg;
    console.bg = bg;
}

void console_put(char c)
{
    if (!console_bypass_lock)
        lock_acquire(&console_lock);

    vga_cell(console.cursor.x, console.cursor.y, vga_entry(c, console.bg, console.fg));
    uart_put(COM1, c);
    console.cursor.x++;

    if (console.cursor.x >= vga_screen_width)
    {
        console.cursor.x = 0;
        console.cursor.y++;
    }

    if (console.cursor.y >= vga_screen_height)
    {
        console.cursor.x = 0;
        console.cursor.y--;
        vga_scroll(console.bg);
    }

    vga_cursor(console.cursor.x, console.cursor.y);

    if (!console_bypass_lock)
        lock_release(&console_lock);
}

void console_puts(string message)
{
    if (!console_bypass_lock)
        lock_acquire(&console_lock);

    bool wait_for_color_id = false;
    
    for(u32 i = 0; message[i]; i++)
    {
        char c = message[i];

        if (wait_for_color_id == true)
        {
            if (c == ASCII_AND)
            {
                vga_cell(console.cursor.x, console.cursor.y, vga_entry(c, console.fg, console.bg));
                uart_put(COM1, c);
                console.cursor.x++;
            }
            else
            {
                char buffer[2] = " ";
                buffer[0] = c;
                console_color_t color = stoi(buffer, 16);
                console.fg = color;
                uart_set_color(COM1, color);
            }

            wait_for_color_id = false;
        }
        else
        {
            switch (c)
            {
                case ASCII_LF:
                    console.cursor.y++;
                    console.cursor.x = 0;
                    break;
                
                case ASCII_AND:
                    wait_for_color_id = true;
                    break;

                case ASCII_HT:
                    console.cursor.x = console.cursor.x + TAB_SIZE - (console.cursor.x % TAB_SIZE);
                    break;
                
                case ASCII_CR:
                    console.cursor.x = 0;
                    break;
                
                default:
                    vga_cell(console.cursor.x, console.cursor.y, vga_entry(c, console.fg, console.bg));
                    console.cursor.x++;
                    break;
            }

            if (c != ASCII_AND)
                uart_put(COM1, c);
        }

        if (console.cursor.x >= vga_screen_width)
        {
            console.cursor.x = 0;
            console.cursor.y++;
        }

        if (console.cursor.y >= vga_screen_height)
        {
            console.cursor.x = 0;
            console.cursor.y--;
            vga_scroll(console.bg);
        }

    }

    vga_cursor(console.cursor.x, console.cursor.y);
    
    if (!console_bypass_lock)
        lock_release(&console_lock);
}

void console_clear()
{
    vga_clear(vga_light_gray, vga_black);
    console.cursor.x = 0;
    console.cursor.y = 0;
    vga_cursor(console.cursor.x, console.cursor.y);
}

void console_read(string buffer, int size)
{
    UNUSED(buffer + size);
}
