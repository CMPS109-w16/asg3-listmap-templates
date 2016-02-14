// $Id: main.cpp,v 1.8 2015-04-28 19:23:13-07 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

string trim(string line){
   size_t start = 0;
   size_t end = 0;

   start = line.find_first_not_of(" ");
   end = line.find_last_not_of(" ");
   line = line.substr(start, end - start + 1);
   return line;
}

void format_line(string line){
   str_str_map test;
   string first; string second;
   size_t equal_sign_pos = 0;

   equal_sign_pos = line.find_first_of("=");
   first = line.substr(0, equal_sign_pos);
   second = line.substr(equal_sign_pos + 1);

   first = trim(first);
   second = trim(second);
   str_str_pair pair(first, second);
   cout << pair << endl;
   test.insert(pair);
}

int main (int argc, char** argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   string line;
   try{
      if(argc == optind){
         for(;;) {
            getline(cin, line);
            if(cin.eof()) break;
            if(line.find_first_of("=") == string::npos);
            else format_line(line);
         }
      }
      else {
         for(char** argp = &argv[optind]; argp != &argv[argc]; ++argp){
            ifstream myfile(*argp);
            if(!myfile)
               throw processing_error("No such file or directory");
            while(getline(myfile, line)) format_line(line);
            myfile.close();
         }
      }
   }catch(processing_error& error){
      complain() << error.what() << endl;
   }
   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}

