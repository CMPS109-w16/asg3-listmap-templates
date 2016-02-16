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

//Removes trailing whitespace from the beginning and end of a string.
string trim(string line) {
   size_t start = 0;
   size_t end = 0;

   start = line.find_first_not_of(" ");
   end = line.find_last_not_of(" ");

   if (line == "") return "";    // Prevents out of range error.
   line = line.substr(start, end - start + 1);
   return line;
}

// Prints out a line of output, consisting of the file name, the line
// number, and the string of output.
void print_line(const string file_name, int& line_num,
         string line_str) {
   cout << file_name << ": " << line_num << ": " << line_str << endl;
}

void format_line(string title, int line_num, string line,
         str_str_map& test) {
   string first { };
   string second { };
   size_t equal_sign_pos { };
   str_str_map::iterator curr;

   ///////////////////////////////////////////////////////////
   // There is no = sign in the command input               //
   ///////////////////////////////////////////////////////////
   //-------------------------------------------------------//
   // CASE: key                                             //
   // Display the given key and its associated value.       //
   //-------------------------------------------------------//
   if (line.find_first_of("=") == string::npos) {
      curr = test.find(line);
      if (curr == test.end())
         cout << line << ": key not found" << endl;
      else {
         print_line(title, line_num, line);
         cout << curr->first << " = " << curr->second << endl;
      }
   } else {
      // If there is an = sign in the command line, this section
      // will parse it.
      // The first section will parse commands where = is at the start.
      // The second section will parse commands where = isn't at start.
      equal_sign_pos = line.find_first_of("=");

      ///////////////////////////////////////////////////////////
      // The first character in line is the = sign.            //
      // CASES: =, = value                                     //
      ///////////////////////////////////////////////////////////
      if (equal_sign_pos == 0) {
         first = "";
         second = line.substr(equal_sign_pos + 1);
         second = trim(second);
         //-------------------------------------------------------//
         // CASE: =                                              //
         // If only = is given, print keys and values.            //
         //-------------------------------------------------------//
         if (second == "") {      // case: =
            print_line(title, line_num, "=");
            curr = test.begin();
            while (curr != test.end()) {
               cout << curr->first << " = " << curr->second << endl;
               ++curr;
            }
            //-------------------------------------------------------//
            // CASE: = VALUE                                        //
            // Output keys whose values match the given value.       //
            //-------------------------------------------------------//
         } else {
            print_line(title, line_num, line);
            curr = test.begin();
            while (curr != test.end()) {
               if (second == curr->second) {
                  cout << curr->first << " = " << curr->second << endl;
               }
               ++curr;
            }
         }
         ///////////////////////////////////////////////////////////
         // There is an = sign, but it is not the first character //
         // CASES: key =, key = value                             //
         ///////////////////////////////////////////////////////////
      } else {
         first = line.substr(0, equal_sign_pos);
         second = line.substr(equal_sign_pos + 1);
         first = trim(first);
         second = trim(second);
         //-------------------------------------------------------//
         // CASE: KEY =                                          //
         // Deletes the given Key and its value.                  //
         //-------------------------------------------------------//
         if (second == "") {
            print_line(title, line_num,
                     "Delete Key and Value and stuff...");
         } else {
            //-------------------------------------------------------//
            // CASE: KEY = VALUE                                    //
            // Insert the Key and Value into the map.                //
            // If the key already exists, change its value.          //
            //-------------------------------------------------------//
            str_str_pair pair(first, second);
            curr = test.insert(pair);
            print_line(title, line_num, line);
            cout << curr->first + " = " + curr->second << endl;
         }
      }
   }
}

int main(int argc, char** argv) {
   sys_info::set_execname(argv[0]);
   scan_options(argc, argv);
   string line;
   str_str_map test;
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
         format_line("-", line_num, line, test);
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
                  // Checks for whitespace before # as first
                  //non-whitespace character to identify the line
                  // as a comment.
               } else if (line.find_first_of("#")
                        <= line.find_first_not_of(" ")) {   // case: #
                  continue;
               }
               format_line(*argp, line_num, line, test);
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

