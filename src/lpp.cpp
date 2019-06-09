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

//#include "error.hpp"
#include "version.hpp"

#include <cstring>
#include <iostream>

void usage()
{
    std::cout << "Usage: lpp <-opts> <file>.logo ..." << std::endl;
    std::cout << "Where -opts is one or more of:" << std::endl;
    std::cout << "  --help | -h     print out this help screen" << std::endl;
    std::cout << "  --version       print out the compiler version" << std::endl;
}


int main(int argc, char * argv[])
{
    std::vector<std::string>    inputs;
    bool                        output_object = false;

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
                        inputs.push_back(argv[i]);
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
                }
            }
            else if(argv[i][1] == '\0')
            {
                inputs.push_back("-"); // stdin
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
                        output_object = true;
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
            // filename
            //
            inputs.push_back(argv[i]);
        }
    }

    if(inputs.empty())
    {
        std::cerr << "error: at least one input file is required." << std::endl;
        return 1;
    }
}


// vim: ts=4 sw=4 et nocindent
