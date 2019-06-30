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
#include "compiler.hpp"

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
        lpp::Compiler::pointer_t compiler(std::make_shared<lpp::Compiler>());
        bool do_compile(true);

        bool stdin_used_up(false);
        for(int i(1); i < argc; ++i)
        {
            auto get_next_param = [&i, argc, argv]()
                {
                    ++i;
                    if(i >= argc)
                    {
                        throw lpp::lpp_error(
                                  "error: not enough information for the "
                                + std::string(argv[i - 1])
                                + " command line option.");
                    }
                    return argv[i];
                };

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
                        std::string arg(argv[i] + 2);
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
                        std::string::size_type const equal(arg.find('='));
                        std::string value;
                        if(equal != std::string::npos)
                        {
                            value = arg.substr(equal + 1);
                            arg = arg.substr(0, equal);
                        }
                        if(arg == "output-object")
                        {
                            parser->set_output_object(true);
                            compiler->set_output_object(true);
                        }
                        else if(arg == "enable-trace")
                        {
                            parser->set_trace(true);
                        }
                        else if(arg == "disable-trace")
                        {
                            parser->set_trace(false);
                        }
                        else if(arg == "do-compile")
                        {
                            do_compile = true;
                        }
                        else if(arg == "no-compile")
                        {
                            do_compile = false;
                        }
                        else if(arg == "keep-l-cpp")
                        {
                            parser->set_keep_l_cpp(true);
                        }
                        else if(arg == "main-cpp")
                        {
                            if(equal != std::string::npos)
                            {
                                compiler->set_main_cpp(value);
                            }
                            else
                            {
                                compiler->set_main_cpp(get_next_param());
                            }
                        }
                        else if(arg == "output")
                        {
                            if(equal != std::string::npos)
                            {
                                compiler->set_output(value);
                            }
                            else
                            {
                                compiler->set_output(get_next_param());
                            }
                        }
                        else if(arg == "include-path")
                        {
                            if(equal != std::string::npos)
                            {
                                compiler->add_include_path(value);
                            }
                            else
                            {
                                compiler->add_include_path(get_next_param());
                            }
                        }
                        else if(arg == "library-path")
                        {
                            if(equal != std::string::npos)
                            {
                                compiler->add_library_path(value);
                            }
                            else
                            {
                                compiler->add_library_path(get_next_param());
                            }
                        }
                        else if(arg == "rpath")
                        {
                            if(equal != std::string::npos)
                            {
                                compiler->add_rpath(value);
                            }
                            else
                            {
                                compiler->add_rpath(get_next_param());
                            }
                        }
                        else if(arg == "library")
                        {
                            if(equal != std::string::npos)
                            {
                                compiler->add_library(value);
                            }
                            else
                            {
                                compiler->add_library(get_next_param());
                            }
                        }
                        else if(arg == "verbose")
                        {
                            parser->set_verbosity(true);
                            compiler->set_verbosity(true);
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
                    switch(argv[i][1])
                    {
                    case 'I':
                        // the -I is special in that the path can
                        // be stuck to it so we need to support that
                        //
                        if(argv[i][2] == '\0')
                        {
                            // we need the next argument
                            //
                            compiler->add_include_path(get_next_param());

                            // NOTE: we allow "-" as an argument of the -I so do
                            //       not prevent such from happening!
                        }
                        else
                        {
                            // the current argument is the important one
                            //
                            compiler->add_include_path(argv[i] + 2);
                        }
                        break;

                    case 'l':
                        if(argv[i][2] == '\0')
                        {
                            compiler->add_library(get_next_param());
                        }
                        else
                        {
                            compiler->add_library(argv[i] + 2);
                        }
                        break;

                    case 'L':
                        if(argv[i][2] == '\0')
                        {
                            compiler->add_library_path(get_next_param());
                        }
                        else
                        {
                            compiler->add_library_path(argv[i] + 2);
                        }
                        break;

                    case 'o':
                        if(argv[i][2] == '\0')
                        {
                            compiler->set_output(get_next_param());
                        }
                        else
                        {
                            compiler->set_output(argv[i] + 2);
                        }
                        break;

                    default:
                        {
                            int const len(std::strlen(argv[i]));
                            for(int j(1); j < len; ++j)
                            {
                                switch(argv[i][j])
                                {
                                case 'c':
                                    parser->set_output_object(true);
                                    compiler->set_output_object(true);
                                    break;

                                case 'E':
                                    parser->set_keep_l_cpp(true);
                                    do_compile = false;
                                    break;

                                case 't':
                                    parser->set_trace(true);
                                    break;

                                case 'h':
                                    usage();
                                    return 0;

                                case 'v':
                                    parser->set_verbosity(true);
                                    compiler->set_verbosity(true);
                                    break;

                                default:
                                    std::cerr << "error: argument \"-"
                                              << argv[i][j]
                                              << "\" is not supported."
                                              << std::endl;
                                    return 1;

                                }
                            }
                        }
                        break;

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
        parser->generate();

        if(!do_compile)
        {
            return 0;
        }

        compiler->set_has_program(parser->has_program());
        return compiler->compile();
    }
    catch(std::logic_error const & e)
    {
        std::cerr << "error: lpp detected a bug in its own implementation; "
                  << e.what()
                  << std::endl;
        return 1;
    }
    catch(lpp::lpp_error const & e)
    {
        // the message of an lpp error is already complete
        //
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch(std::exception const & e)
    {
        std::cerr << "error: unknown standard exception detected: "
                  << e.what()
                  << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "error: unknown exception detected."
                  << std::endl;
        return 1;
    }
}


// vim: ts=4 sw=4 et nocindent
