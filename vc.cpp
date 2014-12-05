//
//  main.cpp
//  ValueCount
//
// Copyright (c) 2014, Ethan Geil
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unistd.h>
#include <vector>

using namespace std;

void usage();
bool valcomp(pair<std::string, unsigned long> *i, pair<std::string, unsigned long> *j);
bool textcomp(pair<std::string, unsigned long> *i, pair<std::string, unsigned long> *j);

int main(int argc, char * const argv[])
{
    int ignore_blank = 0;
    int no_count = 0;
    int ignore_case = 0;
    long length_limit = 0;
    int sort_text = 0;
    int reverse_sort = 0;
    int print_fraction = 0;

    int c;
    int done = 0;
    char *emergencyMemory = new char[1024*100];
    std::ofstream out;
    std::ifstream in;
    std::string line;
    std::string separator = " ";
    std::unordered_map<std::string, unsigned long> counts;
    int line_count = 0;
    double denom;
    
    opterr = 0;
    while (!done && (c = getopt(argc, argv, "+-bfirtuw:")) != -1)
    {
        switch (c)
        {
            case '-':
                done = 1;
                optind--;
                break;
            
            case 'b':
                ignore_blank = 1;
                break;

            case 'f':
                print_fraction = 1;
                break;
                
            case 'i':
                ignore_case = 1;
                break;
            
            case 'r':
                reverse_sort = 1;
                break;
                
            case 't':
                sort_text = 1;
                break;

            case 'u':
                no_count = 1;
                break;
                
            case 'w':
                try {  length_limit = std::stoi(optarg); }
                    catch (std::invalid_argument) { length_limit = -1; }
                
                if (length_limit <= 0)
                {
                    std::cerr << "Invalid length limit value: " << optarg << std::endl;
                }
                break;
            
            case '?':
            default:
                std::cerr << "Unknown option: " << (char)optopt << std::endl;
                usage();
        }
    }
    
    argc -= optind;
    argv += optind;
    
    
    switch (argc)
    {
        case 0:
            in.copyfmt(std::cin);
            in.clear(std::cin.rdstate());
            in.basic_ios<char>::rdbuf(std::cin.rdbuf());
            
            out.copyfmt(std::cout);
            out.clear(std::cout.rdstate());
            out.basic_ios<char>::rdbuf(std::cout.rdbuf());
            break;
        case 1:
            in.open(argv[0]);

            if (!in.good())
            {
                cerr << "1: Couldn't open file for reading: " << argv[0] << endl;
                exit(1);
            }
            
            out.copyfmt(std::cout);
            out.clear(std::cout.rdstate());
            out.basic_ios<char>::rdbuf(std::cout.rdbuf());
            break;
        case 2:
            if (strncmp(argv[0], "-", 1) != 0)
            {
                in.open(argv[0]);
            } 
            else
            {
                in.copyfmt(std::cin);
                in.clear(std::cin.rdstate());
                in.basic_ios<char>::rdbuf(std::cin.rdbuf());
            }

            if (!in.good())
            {
                cerr << "Couldn't open file for reading: " << argv[0] << endl;
                exit(1);
            }

            if (strncmp(argv[1], "-", 1) != 0)
            {
                out.open(argv[1]);
            } 
            else
            {
               out.copyfmt(std::cout);
                out.clear(std::cout.rdstate());
                out.basic_ios<char>::rdbuf(std::cout.rdbuf());
            }
            
            if (!out.good())
            {
                cerr << "Couldn't open file for writing: " << argv[1] << endl;
                exit(1);
            }
            break;
    }
    
    try 
    {
        while (getline(in, line))
        {
            if (ignore_blank && line.length() == 0)
            {
                continue;
            }
            
            if (length_limit > 0)
            {
                line = line.substr(0, length_limit);
            }
            
            if (ignore_case)
            {
                std::transform(line.begin(), line.end(), line.begin(), ::tolower);
            }
            
            counts[line]++;
            line_count++;
        }
        
        std::vector<std::pair<std::string, unsigned long> *> sorted;
        
        for (auto it = counts.cbegin(); it != counts.cend(); it++)
        {
            sorted.push_back((std::pair<std::string, unsigned long> *)&(*it));
        }
        
        if (sort_text)
        {
            std::sort( sorted.begin(), sorted.end(), textcomp );
        }
        else
        {
            std::sort( sorted.begin(), sorted.end(), valcomp );
        }
        
        if (reverse_sort)
        {
            std::reverse(sorted.begin(), sorted.end());
        }

        denom = 1.0/((double)line_count);
        
        for (auto it = sorted.cbegin(); it != sorted.cend(); it++)
        {
            if (!no_count)
            {
                out << (*it)->second << separator;
            }

            if (print_fraction)
            {
                out << ((*it)->second * denom) << separator;
            }

            out << (*it)->first << "\n";
        }
    }
    catch (std::bad_alloc ex)
    {
        delete [] emergencyMemory;
        cerr << "Out of memory" << endl;
        exit(1);
    }

    return 0;
}

void usage()
{
    cerr << "usage: vc [-bfirtu] [-w maxchars] [input [output]]" << endl;
    exit(1);
}

bool valcomp(pair<std::string, unsigned long> *i, pair<std::string, unsigned long> *j)
{
    if (i->second != j->second) return (i->second > j->second);
    return (i->first < j->first);
}

bool textcomp(pair<std::string, unsigned long> *i, pair<std::string, unsigned long> *j)
{
    return (i->first < j->first);
}

