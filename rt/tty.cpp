// lpp -- Logo to binary compiler
// Copyright (C) 2019  Made to Order Software Corporation
// https://www.m2osw.com/lpp
// contact@m2osw.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// self
//
#include "lpp.hpp"

// C++ lib
//
#include <iostream>

// C lib
//
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>


namespace lpp
{


namespace
{



//                            go to (1, 1)      clear screen
//                            |                 |
//                            v                 v
char const g_clear_text[] = { '\x1B', '[', 'H', '\x1B', '[', 'J' };



//                                print cursor position back to us
//                                |
//                                v
char const g_get_cursor_pos[] = { '\x1B', '[', '6', 'n' };



int g_tty = -2;

char g_answer[64] = {};


// TODO: render this one interrupt safe as well so a Ctrl-C doesn't
//       break the whole terminal
//
class safe_terminal
{
public:
    safe_terminal(int tty)
        : f_tty(tty)
    {
        struct termios term;

        tcgetattr(f_tty, &term);
        f_saved_term = term;
        term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(f_tty, TCSANOW, &term);
    }

    ~safe_terminal()
    {
        tcsetattr(f_tty, TCSANOW, &f_saved_term);
    }

private:
    int         f_tty = 0;
    termios     f_saved_term = termios();
};


/** \brief Initialize the lpp TTY.
 *
 * This function is used to initialize a TTY connection between lpp
 * and your terminal. If the isatty() function returns false, then
 * the function does not attempt to connect.
 *
 * If the isatty() returns true, the function tries to connect to
 * the console and may still get an error (-1) and thus not get
 * connected. If that happens, as far as lpp is concerned, you
 * are not connected at all.
 */
int get_tty()
{
    if(g_tty == -2
    && isatty(fileno(stdout)))
    {
        g_tty = open("/dev/tty", O_RDWR);

        if(g_tty >= 0)
        {
        }

    }

    return g_tty;
}


bool read_answer(char stop_char)
{
    int const tty(get_tty());
    if(tty < 0)
    {
        return false;
    }

    for(size_t pos(0);; ++pos)
    {
        if(pos >= sizeof(g_answer))
        {
            // answer too large
            //
            return false;
        }

        if(read(tty, g_answer + pos, 1) != 1)
        {
            // read failed
            //
            return false;
        }

        if(g_answer[pos] == stop_char)
        {
            // got answer as expected
            // do not keep the stop_char...
            //
            g_answer[pos] = '\0';
            return true;
        }
    }
}



} // no name namespace





bool lpp__tty_isatty()
{
    return get_tty() >= 0;
}


bool lpp__tty_flush()
{
    fflush(stdout);
    return true;
}


bool lpp__tty_clear()
{
    int const tty(get_tty());
    if(tty < 0)
    {
        return false;
    }
    lpp__tty_flush();

    if(write(tty, g_clear_text, sizeof(g_clear_text)) != sizeof(g_clear_text))
    {
        return false;
    }

    return true;
}


bool lpp__tty_get_cursor(lpp__integer_t & x, lpp__integer_t & y)
{
    x = 0;
    y = 0;

    int const tty(get_tty());
    if(tty < 0)
    {
        return false;
    }

    safe_terminal terminal(tty);

    if(write(tty, g_get_cursor_pos, sizeof(g_get_cursor_pos)) != sizeof(g_get_cursor_pos))
    {
        return false;
    }

    if(!read_answer('R'))
    {
        return false;
    }

    // answer is [<y>;<x>R
    // (although the read_answer() does not return the 'R'
    //
    if(g_answer[0] != '\x1B'
    || g_answer[1] != '[')
    {
        return false;
    }

    int pos;
    for(pos = 2; g_answer[pos] != ';'; ++pos)
    {
        if(g_answer[pos] < '0'
        || g_answer[pos] > '9')
        {
            return false;
        }
        y = y * 10 + g_answer[pos] - '0';
    }

    for(++pos; g_answer[pos] != '\0'; ++pos)
    {
        if(g_answer[pos] < '0'
        || g_answer[pos] > '9')
        {
            return false;
        }
        x = x * 10 + g_answer[pos] - '0';
    }

    return true;
}


bool lpp__tty_set_cursor(lpp__integer_t x, lpp__integer_t y)
{
    int const tty(get_tty());
    if(tty < 0)
    {
        return false;
    }
    lpp__tty_flush();

    std::string set_position;
    set_position += '\x1B';
    set_position += '[';
    set_position += std::to_string(y);
    set_position += ';';
    set_position += std::to_string(x);
    set_position += 'H';

    if(write(tty, set_position.c_str(), set_position.length()) != static_cast<ssize_t>(set_position.length()))
    {
        return false;
    }

    return true;
}


bool lpp__tty_set_text_colors(lpp__integer_t foreground, lpp__integer_t background)
{
    int const tty(get_tty());
    if(tty < 0)
    {
        return false;
    }
    lpp__tty_flush();

    //
    std::string set_text_colors;
    set_text_colors += '\x1B';
    set_text_colors += '[';
    if(foreground >= 0
    && foreground <= 7)
    {
        set_text_colors += std::to_string(foreground + 30);
    }
    else if(foreground >= 10
         && foreground <= 17)
    {
        set_text_colors += std::to_string(foreground + 80);
    }
    else
    {
        set_text_colors += '3';
        set_text_colors += '9';
    }
    set_text_colors += ';';
    if(background >= 0
    && background <= 7)
    {
        set_text_colors += std::to_string(background + 40);
    }
    else if(background >= 10
         && background <= 17)
    {
        set_text_colors += std::to_string(background + 90);
    }
    else
    {
        set_text_colors += '4';
        set_text_colors += '9';
    }
    set_text_colors += 'm';

    if(write(tty, set_text_colors.c_str(), set_text_colors.length()) != static_cast<ssize_t>(set_text_colors.length()))
    {
        return false;
    }

    return true;
}





} // lpp namespace
// vim: ts=4 sw=4 et nocindent
