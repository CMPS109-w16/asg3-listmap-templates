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

void scan_options(int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt(argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
      case '@':
         traceflags::setflags(optarg);
         break;
      default:
         complain() << "-" << (char) optopt << ": invalid option"
                  << endl;
         break;
      }
   }
}

// Does exactly what it says on the tin.
void print_line(const string file_name, int line_num, string line_str) {
   cout << file_name << ": " << line_num << ": " << line_str << endl;
}

// Finds the two positions with dangling white space, and shaves off
// the white space to return the line.
string trim(string line) {
   size_t start = 0;
   size_t end = 0;

   start = line.find_first_not_of(" ");
   end = line.find_last_not_of(" ");
   line = line.substr(start, end - start + 1);
   return line;
}

// Takes the file or input, and parses it as usable code.
void process_file(string file_name, istream &input_file) {
   str_str_map test { };

   for (int line_num = 1;; ++line_num) {
      string line = "";
      getline(input_file, line);

      // Passing in blank space should return empty lines.
      if (line.size() == 0) {
         print_line(file_name, line_num, "");
         continue;
      }

      // Separate the line into two strings around the = sign.
      string first = "";
      string second = "";
      size_t equal_sign_pos = 0;
      equal_sign_pos = line.find_first_of("=");
      first = line.substr(0, equal_sign_pos);
      second = line.substr(equal_sign_pos + 1);

      // Trim excess whitespace from first and second arguments.
      first = trim(first);
      second = trim(second);

      print_line(file_name, line_num, first + " = " + second);

   }
}

int main(int argc, char** argv) {
   sys_info::set_execname(argv[0]);
   scan_options(argc, argv);
   int EXIT_STATUS = 0;

   //If no arguments stated, run the main program using cin
   if (optind == argc) {
      process_file("-", cin);
   }
   return EXIT_STATUS;
}
