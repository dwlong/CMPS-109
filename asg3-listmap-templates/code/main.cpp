// $Id: main.cpp,v 1.8 2015-04-28 19:23:13-07 - - $
// James Garbagnati
// jgarbagn
// 1354722

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

// This constant literally only exists
// because otherwise I get an error about
// comparing a string literal with argv[i]...
// I mean... I guess it's good form?
const string CIN = "-";

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

// Literally only made because I'm too lazy to type it
// all out every time I need to print it.
void print_pair(const string& key, const string& value) {
   cout << key << " = " << value << endl;
}

void keyvalue(str_str_map& map, const string& filename = CIN, 
   istream& file = cin) {
   for(int line_num = 1;; ++line_num) {
      string line;
      getline(file, line);
      if(file.eof())
         break;
      cout << filename << ": " << line_num << ": " << line << endl;
      line = trim(line);
      
      // Empty / Whitespace-only Line
      if(line.empty())
         continue;
      // Commented Line
      if(line[0] == '#')
         continue;
      
      auto pos = line.find_first_of("=");
      if(pos == string::npos) {
         // key
         auto itor = map.find(line);
         if(itor != map.end())
            print_pair(itor->first, itor->second);
         else
            cout << line << ": key not found" << endl;
      } else {
         string key = trim(line.substr(0, pos));
         string value = trim(line.substr(pos+1));
         str_str_pair pair(key, value);
         if(!key.empty()) {
            if(value.empty()) {
               // key =
               auto itor = map.find(key);
               if(itor != map.end())
                  map.erase(itor);
            } else {
               // key = value
               map.insert(pair);
               print_pair(key, value);
            }
         } else {
            if(value.empty()) {
               // =
               for(auto itor = map.begin(); itor != map.end(); ++itor)
                  print_pair(itor->first, itor->second);
            } else {
               // = value
               for(auto itor = map.begin(); itor != map.end(); ++itor)
                  if(itor->second == value)
                     print_pair(itor->first, itor->second);
            }
         }
      }
   }
}

int main (int argc, char** argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   str_str_map listmap;
   
   if(argc == 1)
      keyvalue(listmap);
   for(int i = 1; i < argc; ++i) {
      if(argv[i] == CIN) {
         keyvalue(listmap);
      } else {
         ifstream file(argv[i]);
         // Check to see if file exists.
         if(file.fail()) {
            complain() << argv[i] << ": no such file" << endl;
            continue;
         }
         keyvalue(listmap, argv[i], file);
         file.close();
      }
   }
   
   return sys_info::get_exit_status();
}

