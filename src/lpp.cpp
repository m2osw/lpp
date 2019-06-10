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

#include "parser.hpp"

#include "version.hpp"

#include <cstring>
#include <fstream>
#include <iostream>


// this is the GPL v3 in a string
//
#include "license.cpp"

void usage()
{
    std::cout << "Usage: lpp <-opts> <file>.logo ...\n\n"

"Where -opts is one or more of:\n"
"  --copyright              print out this program copyright notice\n"
"  --enable-trace           instruments the code so tracing works (for debug)\n"
"  --disable-trace          do not instruments the code for tracing (will be faster)\n"
"  --help | -h              print out this help screen\n"
"  --license                print out this program full license\n"
"  --output-object          generate a .o as the output\n"
"  --version                print out the compiler version\n"

;
}


int main(int argc, char * argv[])
{
    try
    {
        lpp::Parser::pointer_t parser(std::make_shared<lpp::Parser>());

        bool stdin_used_up(false);
        for(int i(1); i < argc; ++i)
        {
            if(argv[i][0] == '-')
            {
                if(argv[i][1] == '-')
                {
                    if(argv[i][2] == '\0')
                    {
                        for(++i; i < argc; ++i)
                        {
                            lpp::Lexer::pointer_t lexer(std::make_shared<lpp::Lexer>(argv[i]));
                            parser->add_lexer(lexer);
                        }
                    }
                    else
                    {
                        // long argument
                        //
                        std::string const arg(argv[i] + 2);
                        if(arg == "help")
                        {
                            usage();
                            return 0;
                        }
                        if(arg == "version")
                        {
                            std::cout << LPP_VERSION_STRING << std::endl;
                            return 0;
                        }
                        if(arg == "copyright")
                        {
                            // simple copyright notice
                            //
                            std::cout << "lpp -- Copyright (c) 2019  Made to Order Software Corporation\n"
                                         "This program comes with ABSOLUTELY NO WARRANTY; for details type `lpp --license'.\n"
                                         "This is free software, and you are welcome to redistribute it\n"
                                         "under certain conditions; type `lpp --license' for details.\n";
                            return 0;
                        }
                        if(arg == "license")
                        {
                            std::cout << "lpp -- Copyright (c) 2019  Made to Order Software Corporation\n"
                                         "This program is covered by the GPL v3 license as follow:\n"
                                         "\n";
                            std::cout << license;
                            return 0;
                        }
                        if(arg == "output-object")
                        {
                            parser->set_output_object(true);
                        }
                        else if(arg == "enable-trace")
                        {
                            parser->set_trace(true);
                        }
                        else if(arg == "disable-trace")
                        {
                            parser->set_trace(false);
                        }
                    }
                }
                else if(argv[i][1] == '\0')
                {
                    if(stdin_used_up)
                    {
                        std::cerr << "error: the \"-\" filename can be used only once."
                                  << std::endl;
                        return 1;
                    }
                    stdin_used_up = true;

                    lpp::Lexer::pointer_t lexer(std::make_shared<lpp::Lexer>("-")); // stdin
                    parser->add_lexer(lexer);
                }
                else
                {
                    // short arguments
                    //
                    int const len(std::strlen(argv[i]));
                    for(int j(1); j < len; ++j)
                    {
                        switch(argv[i][j])
                        {
                        case 'c':
                            parser->set_output_object(true);
                            break;

                        case 't':
                            parser->set_trace(true);
                            break;

                        case 'h':
                            usage();
                            return 0;

                        }
                    }
                }
            }
            else
            {
                // direct filename
                //
                lpp::Lexer::pointer_t lexer(std::make_shared<lpp::Lexer>(argv[i]));
                parser->add_lexer(lexer);
            }
        }

        parser->parse();
    }
    catch(std::logic_error const & e)
    {
        std::cerr << "error: lpp detected a bug in its own implementation; "
                  << e.what()
                  << std::endl;
    }
    catch(lpp::lpp_error const & e)
    {
        // the message of an lpp error is already complete
        //
        std::cerr << e.what() << std::endl;
    }
    catch(std::exception const & e)
    {
        std::cerr << "error: unknown standard exception detected: "
                  << e.what()
                  << std::endl;
    }
    catch(...)
    {
        std::cerr << "error: unknown exception detected."
                  << std::endl;
    }
}


// vim: ts=4 sw=4 et nocindent
