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
#include <fstream>
#include <iostream>
#include <sstream>

// C lib
//
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>



namespace lpp
{


namespace
{



struct lpp__file
{
    typedef std::shared_ptr<lpp__file>              pointer_t;
    typedef std::map<std::string, pointer_t>        map_t;
    typedef std::basic_istream<char> *              ifile_ptr_t;  // a bare pointer before we want to also point to cin and cannot use a smart pointer for those
    typedef std::basic_ostream<char> *              ofile_ptr_t;  // a bare pointer before we want to also point to cout and cannot use a smart pointer for those

    ~lpp__file()
    {
        if(f_istream != &std::cin
        && f_ostream != &std::cout)
        {
            if(static_cast<std::basic_ios<char> *>(f_istream) == static_cast<std::basic_ios<char> *>(f_ostream))
            {
                delete f_istream;
            }
            else
            {
                delete f_istream;
                delete f_ostream;
            }
        }
    }

    open_mode_t         f_mode = open_mode_t::OPEN_MODE_READ;
    bool                f_is_reader = false;
    bool                f_is_writer = false;
    std::string         f_filename = std::string();
    ifile_ptr_t         f_istream = ifile_ptr_t();
    ofile_ptr_t         f_ostream = ofile_ptr_t();
};

std::string             g_prefix = std::string();
lpp__file::map_t        g_files = lpp__file::map_t();
lpp__file::pointer_t    g_reader = lpp__file::pointer_t();
lpp__file::pointer_t    g_writer = lpp__file::pointer_t();
lpp__file::pointer_t    g_terminal_out = lpp__file::pointer_t();
lpp__file::pointer_t    g_terminal_in = lpp__file::pointer_t();


lpp__file::pointer_t get_file(lpp__context::pointer_t context, std::string const & filename, open_mode_t mode)
{
    switch(mode)
    {
    case open_mode_t::OPEN_MODE_READ:
        if(filename.empty())
        {
            if(g_reader == nullptr)
            {
                if(g_terminal_in == nullptr)
                {
                    g_terminal_in = std::make_shared<lpp__file>();
                    g_terminal_in->f_mode = open_mode_t::OPEN_MODE_READ;
                    g_terminal_in->f_is_reader = true;
                    g_terminal_in->f_istream = &std::cin;
                }
                return g_terminal_in;
            }
            return g_reader;
        }
        break;

    case open_mode_t::OPEN_MODE_WRITE:
        if(filename.empty())
        {
            if(g_writer == nullptr)
            {
                if(g_terminal_out == nullptr)
                {
                    g_terminal_out = std::make_shared<lpp__file>();
                    g_terminal_out->f_mode = open_mode_t::OPEN_MODE_WRITE;
                    g_terminal_out->f_is_writer = true;
                    g_terminal_out->f_ostream = &std::cout;
                }
                return g_terminal_out;
            }
            return g_writer;
        }
        break;

    default:
        throw std::logic_error("invalid mode ("
                             + to_string(mode)
                             + ") for a call to get_file().");

    }

    std::string const name(g_prefix + filename);
    auto const & it(g_files.find(name));
    if(it == g_files.end())
    {
        throw lpp__error(context
                       , lpp__error_code_t::ERROR_CODE_FILE_IS_NOT_OPEN
                       , "error"
                       , "file \"" + filename + "\" not found.");
    }

    bool valid(false);
    switch(it->second->f_mode)
    {
    case open_mode_t::OPEN_MODE_UPDATE:
        valid = true;
        break;

    case open_mode_t::OPEN_MODE_READ:
        valid = mode == open_mode_t::OPEN_MODE_READ;
        break;

    case open_mode_t::OPEN_MODE_APPEND:
    case open_mode_t::OPEN_MODE_WRITE:
    case open_mode_t::OPEN_MODE_WRITE_BUFFER:
        valid = mode == open_mode_t::OPEN_MODE_WRITE;
        break;

    }
    if(!valid)
    {
        throw lpp__error(context
                       , lpp__error_code_t::ERROR_CODE_FILE_ACCESS_MISMATCH
                       , "error"
                       , "file \"" + filename + " is not open for this function to work against it.");
    }

    return it->second;
}



// tables with various terminal codes
// https://wiki.bash-hackers.org/scripting/terminalcodes


//                            go to (1, 1)      clear screen
//                            |                 |
//                            v                 v
char const g_clear_text[] = { '\x1B', '[', 'H', '\x1B', '[', 'J' };



//                                print cursor position back to us
//                                |
//                                v
char const g_get_cursor_pos[] = { '\x1B', '[', '6', 'n' };



//                                 print cursor position back to us
//                                 |
//                                 v
char const g_remove_standout[] = { '\x1B', '[', '0', 'm' };



//                          print cursor position back to us
//                          |
//                          v
char const g_standout[] = { '\x1B', '[', '7', 'm' };



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
 * This function checks whether stdout is a TTY, if so its file number is
 * saved in the g_tty variable and it gets returned.
 *
 * If the function returns -1, then you are not connected to a TTY. Any
 * other number and you can send TTY like characters.
 *
 * \return The TTY file number of -1 if stdout is not a tty.
 */
int get_tty()
{
    if(g_tty == -2)
    {
        g_tty = fileno(stdout);
        if(!isatty(g_tty))
        {
            g_tty = -1;
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





bool lpp__tty_flush()
{
    if(get_tty() >= 0)
    {
        fflush(stdout);
        return true;
    }

    return false;
}


bool lpp__tty_isatty()
{
    return get_tty() >= 0;
}


bool lpp__tty_clear_screen()
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


bool lpp__tty_set_standout(bool on)
{
    int const tty(get_tty());
    if(tty < 0)
    {
        return false;
    }
    lpp__tty_flush();

    if(on)
    {
        if(write(tty, g_standout, sizeof(g_standout)) != sizeof(g_standout))
        {
            return false;
        }
    }
    else
    {
        if(write(tty, g_remove_standout, sizeof(g_remove_standout)) != sizeof(g_remove_standout))
        {
            return false;
        }
    }

    return true;
}



lpp__value::vector_t lpp__file_list()
{
    lpp__value::vector_t list;

    for(auto const & p : g_files)
    {
        list.push_back(std::make_shared<lpp__value>(p.first));
    }

    return list;
}


lpp__value::pointer_t lpp__file_info(lpp__context::pointer_t context, std::string const & filename)
{
    std::string const name(g_prefix + filename);
    struct stat s;
    if(::stat(name.c_str(), &s) != 0)
    {
        throw lpp__error(context
                       , lpp__error_code_t::ERROR_CODE_FILE_SYSTEM_ERROR
                       , "error"
                       , "could not get information of file \"" + filename + "\".");
    }

    lpp__value::vector_t list;
    lpp__value::pointer_t prop(std::make_shared<lpp__value>(list));

    prop->add_prop("uid", std::make_shared<lpp__value>(static_cast<lpp__integer_t>(s.st_uid)));
    passwd * pw(getpwuid(s.st_uid));
    if(pw != nullptr)
    {
        prop->add_prop("user", std::make_shared<lpp__value>(std::string(pw->pw_name)));
    }

    prop->add_prop("gid", std::make_shared<lpp__value>(static_cast<lpp__integer_t>(s.st_gid)));
    group * gr(getgrgid(s.st_gid));
    if(gr != nullptr)
    {
        prop->add_prop("user", std::make_shared<lpp__value>(std::string(gr->gr_name)));
    }

    prop->add_prop("mode", std::make_shared<lpp__value>(static_cast<lpp__integer_t>(s.st_mode)));

    lpp__value::vector_t modes;
    if(s.st_mode & S_ISUID)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("suid")));
    }
    if(s.st_mode & S_ISGID)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("sgid")));
    }
    if(s.st_mode & S_ISVTX)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("svtx")));
    }
    if(s.st_mode & S_IRUSR)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("rusr")));
    }
    if(s.st_mode & S_IWUSR)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("wusr")));
    }
    if(s.st_mode & S_IXUSR)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("xusr")));
    }
    if(s.st_mode & S_IRGRP)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("rgrp")));
    }
    if(s.st_mode & S_IWGRP)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("wgrp")));
    }
    if(s.st_mode & S_IXGRP)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("xgrp")));
    }
    if(s.st_mode & S_IROTH)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("roth")));
    }
    if(s.st_mode & S_IWOTH)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("woth")));
    }
    if(s.st_mode & S_IXOTH)
    {
        modes.push_back(std::make_shared<lpp__value>(std::string("xoth")));
    }
    prop->add_prop("modes", std::make_shared<lpp__value>(modes));

    prop->add_prop("size", std::make_shared<lpp__value>(static_cast<lpp__integer_t>(s.st_size)));
    prop->add_prop("atime", std::make_shared<lpp__value>(static_cast<lpp__float_t>(s.st_atim.tv_sec) + static_cast<lpp__float_t>(s.st_atim.tv_nsec) / 1000000000.0));
    prop->add_prop("mtime", std::make_shared<lpp__value>(static_cast<lpp__float_t>(s.st_mtim.tv_sec) + static_cast<lpp__float_t>(s.st_mtim.tv_nsec) / 1000000000.0));
    prop->add_prop("ctime", std::make_shared<lpp__value>(static_cast<lpp__float_t>(s.st_ctim.tv_sec) + static_cast<lpp__float_t>(s.st_ctim.tv_nsec) / 1000000000.0));

    return prop;
}


void lpp__set_prefix(std::string const & path)
{
    if(path.empty())
    {
        g_prefix.clear();
    }
    else
    {
        if(path.back() == '/')
        {
            g_prefix = path;
        }
        else
        {
            g_prefix = path + '/';
        }
    }
}


std::string lpp__get_prefix()
{
    if(g_prefix.empty())
    {
        return std::string();
    }
    return g_prefix.substr(0, g_prefix.length() - 1);
}


bool lpp__filep(lpp__context::pointer_t context, std::string const & filename)
{
    static_cast<void>(context);
    return access((g_prefix + filename).c_str(), R_OK);
}


bool lpp__open_file(lpp__context::pointer_t context, std::string const & filename, open_mode_t mode)
{
    lpp__file::pointer_t file(std::make_shared<lpp__file>());

    file->f_mode = mode;

    if(mode == open_mode_t::OPEN_MODE_WRITE_BUFFER)
    {
        file->f_filename = filename;
        file->f_ostream = new std::stringstream;
    }
    else
    {
        file->f_filename = g_prefix + filename;
        std::fstream * ptr(new std::fstream);
        file->f_istream = ptr;
        file->f_ostream = ptr;

        std::ios_base::openmode om(std::ios_base::binary);
        switch(mode)
        {
        case open_mode_t::OPEN_MODE_APPEND:
            om |= std::ios_base::app | std::ios_base::out;
            break;

        case open_mode_t::OPEN_MODE_READ:
            om |= std::ios_base::in;
            break;

        case open_mode_t::OPEN_MODE_UPDATE:
            om |= std::ios_base::ate | std::ios_base::in | std::ios_base::out;
            break;

        case open_mode_t::OPEN_MODE_WRITE:
            om |= std::ios_base::trunc | std::ios_base::out;
            break;

        case open_mode_t::OPEN_MODE_WRITE_BUFFER:
            throw std::logic_error("OPEN_MODE_WRITE_BFUFER was caught in the other if() block");

        }

        ptr->open(file->f_filename, om);
        if(!ptr->is_open())
        {
            throw lpp__error(context
                           , lpp__error_code_t::ERROR_CODE_FILE_SYSTEM_ERROR
                           , "error"
                           , "could not open file \"" + filename + "\".");
        }
    }

    // IMPORTANT NOTE: This automatically closes a previous instance if
    //                 such existed, unless that instance is the reader
    //                 or writer in which case we probably want to generate
    //                 an error (TODO verify that)
    //
    g_files[file->f_filename] = file;

    return true;
}


void lpp__flush_file(lpp__context::pointer_t context, std::string const & filename)
{
    lpp__file::pointer_t file(get_file(context, filename, open_mode_t::OPEN_MODE_READ));
    if(file->f_ostream != nullptr)
    {
        file->f_ostream->flush();
    }
}


lpp__value::pointer_t lpp__read_file(lpp__context::pointer_t context, std::string const & filename, read_mode_t mode)
{
    struct read_data
    {
        std::string get_word()
        {
            char c;
            std::string word;
            switch(f_file->f_istream->peek())
            {
            case '(':
            case ')':
            case '[':
            case ']':
            case '+':
            case '-':
            case '*':
            case '/':
            case '<':
            case '>':
            case '=':
            case '#':
                f_file->f_istream->get(c);
                word += c;
                switch(c)
                {
                case '<':
                    switch(f_file->f_istream->peek())
                    {
                    case '>':
                    case '=':
                        f_file->f_istream->get(c);
                        word += c;
                        break;

                    }
                    break;

                case '>':
                    if(f_file->f_istream->peek() == '=')
                    {
                        f_file->f_istream->ignore();
                        word += '=';
                    }
                    break;

                }
                return word;

            }
            for(;;)
            {
                f_file->f_istream->get(c);
                if(!f_file->f_istream)
                {
                    return word;
                }
                if(c == '|')
                {
                    for(;;)
                    {
                        f_file->f_istream->get(c);
                        if(!f_file->f_istream)
                        {
                            return word;
                        }
                        if(c == '|')
                        {
                            break;
                        }
                        if(c == '\\')
                        {
                            f_file->f_istream->get(c);
                            if(!f_file->f_istream)
                            {
                                return word;
                            }
                        }
                        word += c;
                    }
                }
                else
                {
                    switch(c)
                    {
                    case '\t':
                    case ' ':
                        // no need to unget spaces
                        //
                        if(word.empty())
                        {
                            // no word yet, read more
                            //
                            continue;
                        }
                        return word;

                    case '\r':
                    case '\n':
                        // these are important here since we read lines
                        // one at a time (unless '[' and ']' are used
                        // and then a list can appear on multiple lines)
                        //
                        if(word.empty())
                        {
                            if(c == '\r'
                            && f_file->f_istream->peek() == '\n')
                            {
                                f_file->f_istream->ignore();
                            }
                            word += '\n';
                            return word;
                        }
                        f_file->f_istream->unget();
                        return word;

                    case '(':
                    case ')':
                    case '[':
                    case ']':
                        f_file->f_istream->unget();
                        return word;

                    }
                    if(c == '\\')
                    {
                        f_file->f_istream->get(c);
                        if(!f_file->f_istream)
                        {
                            return word;
                        }
                    }
                    word += c;
                }
            }
        }

        void get_expr(int count, lpp__value::vector_t list)
        {
            list.push_back(std::make_shared<lpp__value>(std::string("(")));
            for(;;)
            {
                std::string const word(get_word());
                if(word.empty())
                {
                    throw lpp__error(f_context
                                   , lpp__error_code_t::ERROR_CODE_MISSING_CLOSE_PARENTHESIS
                                   , "error"
                                   , "reach end of file named \"" + f_file->f_filename + "\" before ')' was found.");
                }
                else if(word != "\n")
                {
                    list.push_back(std::make_shared<lpp__value>(word));
                    if(word == ")")
                    {
                        --count;
                        if(count == 0)
                        {
                            return;
                        }
                    }
                    else if(word == "(")
                    {
                        ++count;
                        if(count >= 1000)
                        {
                            throw lpp__error(f_context
                                           , lpp__error_code_t::ERROR_CODE_TOO_MANY_PARENTHESIS
                                           , "error"
                                           , "too many '(' found in the file named \"" + f_file->f_filename + "\".");
                        }
                    }
                }
            }
        }

        lpp__value::pointer_t get_list(lpp__context::pointer_t context, int count, bool eol)
        {
            if(count >= 1000)
            {
                throw lpp__error(f_context
                               , lpp__error_code_t::ERROR_CODE_TOO_MANY_LISTS
                               , "error"
                               , "too many '[' found without ']' in file named \"" + f_file->f_filename + "\".");
            }

            lpp__value::vector_t list;
            for(;;)
            {
                std::string const word(get_word());
                if(word.empty()
                || word == "\n")
                {
                    if(eol
                    && count == 0)
                    {
                        return std::make_shared<lpp__value>(list);
                    }

                    if(word.empty())
                    {
                        throw lpp__error(f_context
                                       , lpp__error_code_t::ERROR_CODE_MISSING_CLOSE_LIST
                                       , "error"
                                       , "not enough ']' found before the end of the file named \"" + f_file->f_filename + "\".");
                    }
                }
                else if(word == "[")
                {
                    list.push_back(get_list(context, count + 1, eol));
                }
                else if(word == "]")
                {
                    if(count == 0)
                    {
                        throw lpp__error(f_context
                                       , lpp__error_code_t::ERROR_CODE_UNEXPECTED_CLOSE_LIST
                                       , "error"
                                       , "found standalone ']' in the file named \"" + f_file->f_filename + "\".");
                    }
                    return std::make_shared<lpp__value>(list);
                }
                else if(word == "(")
                {
                    get_expr(1, list);
                }
                else if(word == ")")
                {
                    throw lpp__error(f_context
                                   , lpp__error_code_t::ERROR_CODE_UNEXPECTED_CLOSE_PARENTHESIS
                                   , "error"
                                   , "found standalone ')' in the file named \"" + f_file->f_filename + "\".");
                }
                else
                {
                    list.push_back(std::make_shared<lpp__value>(word));
                }
            }
        }

        lpp__file::pointer_t        f_file = lpp__file::pointer_t();
        lpp__context::pointer_t     f_context = lpp__context::pointer_t();
    };

    read_data in;

    in.f_file = get_file(context, filename, open_mode_t::OPEN_MODE_READ);
    switch(mode)
    {
    case read_mode_t::READ_MODE_BYTE:
        {
            char c;
            in.f_file->f_istream->get(c);
            if(!in.f_file->f_istream)
            {
                return std::make_shared<lpp__value>(static_cast<lpp__integer_t>(-1));
            }
            return std::make_shared<lpp__value>(static_cast<lpp__integer_t>(static_cast<std::uint8_t>(c)));
        }

    case read_mode_t::READ_MODE_LINE:
        {
            std::string line;
            for(;;)
            {
                char c;
                in.f_file->f_istream->get(c);
                if(c == '\r')
                {
                    if(in.f_file->f_istream->peek() == '\n')
                    {
                        in.f_file->f_istream->ignore();
                    }
                    c = '\n';
                }
                if(c == '\n'
                || !in.f_file->f_istream)
                {
                    return std::make_shared<lpp__value>(line);
                }
                line += c;
            }
        }

    case read_mode_t::READ_MODE_CHAR:
        // read one UTF-8 character, stop as soon as an invalid byte is
        // found (so the result may not really be valid UTF-8)
        {
            std::string s;
            char c('\0');
            in.f_file->f_istream->get(c);
            if(!in.f_file->f_istream)
            {
                // we reached the end of the file
                // (TODO: or an error occurred)
                //
                return std::make_shared<lpp__value>(s);
            }
            s += c;
            int l(1);
            if(static_cast<unsigned char>(c) >= 0xF0)
            {
                l = 4;
            }
            else if(static_cast<unsigned char>(c) >= 0xE0)
            {
                l = 3;
            }
            else if(static_cast<unsigned char>(c) >= 0xC0)
            {
                l = 2;
            }
            for(int i(1); i < l; ++i)
            {
                c = in.f_file->f_istream->peek();
                if(static_cast<unsigned char>(c) >= 0x80
                && static_cast<unsigned char>(c) <  0xC0)
                {
                    in.f_file->f_istream->ignore();
                    s += c;
                }
                else
                {
                    c = static_cast<char>(0x80);
                    for(; i < l; ++i)
                    {
                        s += c;
                    }
                    break;
                }
            }
            return std::make_shared<lpp__value>(s);
        }

    case read_mode_t::READ_MODE_WORD:
        for(;;)
        {
            std::string const word(in.get_word());
            if(word != "\n")
            {
                return std::make_shared<lpp__value>(word);
            }
        }

    case read_mode_t::READ_MODE_LIST:
        {
            char c('\0');
            do
            {
                c = in.f_file->f_istream->peek();
            }
            while(c == ' '
               || c == '\t'
               || c == '\n'
               || c == '\r');
            if(c == '[')
            {
                // read one list
                //
                in.f_file->f_istream->ignore();
                return in.get_list(context, 1, false);
            }

            // read one line (of words)
            //
            return in.get_list(context, 0, true);
        }

    default:
        return lpp__value::pointer_t();

    }
}


lpp__integer_t lpp__read_position(lpp__context::pointer_t context, std::string const & filename)
{
    lpp__file::pointer_t file(get_file(context, filename, open_mode_t::OPEN_MODE_READ));
    return file->f_istream->tellg();
}


void lpp__set_read_position(lpp__context::pointer_t context, std::string const & filename, lpp__integer_t pos)
{
    lpp__file::pointer_t file(get_file(context, filename, open_mode_t::OPEN_MODE_READ));
    file->f_istream->seekg(pos);
}


void lpp__set_reader(lpp__context::pointer_t context, std::string const & filename)
{
    if(g_reader != nullptr)
    {
        g_reader->f_is_reader = false;
        g_reader.reset();
    }
    g_reader = get_file(context, filename, open_mode_t::OPEN_MODE_READ);
    g_reader->f_is_reader = true;
}


std::string lpp__get_reader()
{
    if(g_reader == nullptr)
    {
        return std::string();
    }

    return g_reader->f_filename;
}


void lpp__write_file(lpp__context::pointer_t context, std::string const & filename, lpp__value::pointer_t data)
{
    lpp__write_file(context, filename, data->to_string());
}


void lpp__write_file(lpp__context::pointer_t context, std::string const & filename, std::string const & data)
{
    lpp__file::pointer_t file(get_file(context, filename, open_mode_t::OPEN_MODE_WRITE));
    file->f_ostream->write(data.c_str(), data.length());
    if(!file->f_ostream)
    {
        throw lpp__error(context
                       , lpp__error_code_t::ERROR_CODE_FILE_SYSTEM_ERROR
                       , "error"
                       , "I/O error when attempting to erase \"" + filename + "\".");
    }
}


lpp__integer_t lpp__write_position(lpp__context::pointer_t context, std::string const & filename)
{
    lpp__file::pointer_t file(get_file(context, filename, open_mode_t::OPEN_MODE_WRITE));
    return file->f_ostream->tellp();
}


void lpp__set_write_position(lpp__context::pointer_t context, std::string const & filename, lpp__integer_t pos)
{
    lpp__file::pointer_t file(get_file(context, filename, open_mode_t::OPEN_MODE_WRITE));
    file->f_ostream->seekp(pos);
}


void lpp__set_writer(lpp__context::pointer_t context, std::string const & filename)
{
    if(g_writer != nullptr)
    {
        g_writer->f_is_writer = false;
        g_writer.reset();
    }
    g_writer = get_file(context, filename, open_mode_t::OPEN_MODE_WRITE);
    g_writer->f_is_writer = true;
}


std::string lpp__get_writer()
{
    if(g_writer == nullptr)
    {
        return std::string();
    }

    return g_writer->f_filename;
}


bool lpp__eofp(lpp__context::pointer_t context, std::string const & filename)
{
    lpp__file::pointer_t file(get_file(context, filename, open_mode_t::OPEN_MODE_READ));

    // TODO: the basic_ios::eof() is not correct for Logo, we probably need
    //       to have the file size and do a READPOS >= SIZE
    //
    return file->f_istream->eof();
}


void lpp__close_file(lpp__context::pointer_t context, std::string const & filename, bool force)
{
    auto const & it(g_files.find(g_prefix + filename));
    if(it != g_files.end())
    {
        if(it->second == g_reader)
        {
            if(!force)
            {
                return;
            }
            g_reader.reset();
        }
        if(it->second == g_writer)
        {
            if(!force)
            {
                return;
            }
            g_writer.reset();
        }

        // auto-set that variable on close
        //
        if(it->second->f_mode == open_mode_t::OPEN_MODE_WRITE_BUFFER)
        {
            std::stringstream * ss(dynamic_cast<std::stringstream *>(it->second->f_ostream));
            if(ss == nullptr)
            {
                throw std::logic_error("somehow the write-buffer is not an std::stringstream");
            }
            std::string const buffer(ss->str());
            context->set_thing(it->second->f_filename
                             , std::make_shared<lpp__value>(buffer));
        }

        g_files.erase(it);
    }
}


void lpp__erase_file(lpp__context::pointer_t context, std::string const & filename)
{
    std::string const name(g_prefix + filename);
    auto const & it(g_files.find(name));
    if(it != g_files.end())
    {
        throw lpp__error(context
                       , lpp__error_code_t::ERROR_CODE_FILE_IS_OPEN
                       , "error"
                       , "\"" + filename + "\" is currently opened so it can't be erased.");
    }

    if(unlink(name.c_str()) != 0)
    {
        throw lpp__error(context
                       , lpp__error_code_t::ERROR_CODE_FILE_SYSTEM_ERROR
                       , "error"
                       , "I/O error when attempting to erase \"" + filename + "\".");
    }
}


std::string to_string(open_mode_t mode)
{
    switch(mode)
    {
    case open_mode_t::OPEN_MODE_APPEND:
        return "append";

    case open_mode_t::OPEN_MODE_READ:
        return "read";

    case open_mode_t::OPEN_MODE_UPDATE:
        return "update";

    case open_mode_t::OPEN_MODE_WRITE:
        return "write";

    case open_mode_t::OPEN_MODE_WRITE_BUFFER:
        return "write-buffer";

    }

    return "<unknown:" + std::to_string(static_cast<int>(mode)) + ">";
}


} // lpp namespace
// vim: ts=4 sw=4 et nocindent
