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

// Removes trailing whitespace from the beginning and end of a string.
string trim(string line) {
   size_t start = 0;
   size_t end = 0;

   start = line.find_first_not_of(" ");
   end = line.find_last_not_of(" ");
//    Some single-character lines need to be handled specially.
   if (start == end && start != 0 && end != 0) {
      if (line == "#") {
         return "#";
      } else if (line == "=") {
         return "=";
      } else if (line == "") {
         return "";
      }
   }
   line = line.substr(start, end - start + 1);
   return line;
}

// Prints out a line of output, consisting of the file name, the line
// number, and the string of output.
void print_line(const string file_name, int& line_num, string line_str) {
   cout << file_name << ": " << line_num << ": " << line_str << endl;
}

void format_line(string title, int line_num, string line) {
   str_str_map test;
   string first {};
   string second {};
   size_t equal_sign_pos { };

   // Doesn't do anything for empty line commands.
   if (line == "") {    // case: <whitespace>
      return;
      // Doesn't do anything for comments either.
   } else if (line.find_first_of("#") == 0) {   // case: #
      return;
   }

   // If there's no = sign at all...
   if (line.find_first_of("=") == string::npos) {   // case: key
      print_line(title, line_num, "Search for Keys and stuff");
   } else {
      // If there is an = sign somewhere, first find where it is.
      equal_sign_pos = line.find_first_of("=");

      // If the equals is the first character...
      if (equal_sign_pos == 0) {
         first = "";
         second = line.substr(equal_sign_pos + 1);
         second = trim(second);
         // If = is alone without values, show all keys and values.
         if (second == "") {      // case: =
            print_line(title, line_num, "Show Everything and stuff...");
         // If = is followed by a value, show the keys with that value.
         } else {       // case: = value
            print_line(title, line_num, "Find Keys from Values and stuff...");
         }
      // If equals is not the first character...
      } else {
         first = line.substr(0, equal_sign_pos);
         second = line.substr(equal_sign_pos + 1);
         first = trim(first);
         second = trim(second);
         // If there is a key followed by =, show the value of that key.
         if (second == "") {    // case: key =
            print_line(title, line_num, "Show Value for Key and stuff...");
         // Final case is a key, =, and a value in that order.
         // Insert the key and value into the map.
         } else {    // case: key = value
            str_str_pair pair(first, second);
            test.insert(pair);
            print_line(title, line_num, first + " + " + second);
         }
      }
      // Debugging prints. Remove later.
      str_str_pair pair(first, second);
      cout << pair << endl;
   }
}

int main(int argc, char** argv) {
   sys_info::set_execname(argv[0]);
   scan_options(argc, argv);
   string line;
   // If no arguments are specified when executing the program,
   // the program will take cin inputs via command line.
   if (argc == optind) {
      int line_num = 1;
      for (;;) {
         getline(cin, line);
         if (cin.eof()) break;
         // Doesn't do anything for empty line commands.
         if (line == "") {    // case: <whitespace>
            continue;
            // Doesn't do anything for comments either.
            // Checks for whitespace before # as first non-whitespace
            // character to identify the line as a comment.
         } else if (line.find_first_of("#")
                  <= line.find_first_not_of(" ")) {   // case: #
            continue;
         }
         format_line("-", line_num, line);
         ++line_num;
      }
   }
   // If arguments are specified when executing the program,
   // the program will treat the arguments as file names, and input the
   // lines within those files as command inputs.
   else {
      for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
         int line_num = 1;
         try {
            ifstream myfile(*argp);
            if (!myfile)
               throw processing_error("No such file or directory");
            while (getline(myfile, line)) {
               // Doesn't do anything for empty line commands.
               if (line == "") {    // case: <whitespace>
                  continue;
                  // Doesn't do anything for comments either.
                  // Checks for whitespace before # as first non-whitespace
                  // character to identify the line as a comment.
               } else if (line.find_first_of("#")
                        <= line.find_first_not_of(" ")) {   // case: #
                  continue;
               }
               format_line(*argp, line_num, line);
               ++line_num;
            }
            myfile.close();
         } catch (processing_error& error) {
            complain() << error.what() << endl;
         }
      }
   }
   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}

