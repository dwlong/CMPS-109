// $Id: file_sys.cpp,v 1.1 2016-01-27 00:02:51-08 - - $
// James Garbagnati
// jgarbagnati
// 1354722

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

/**
 * Inode State
 */

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

const string& inode_state::prompt() { return prompt_; }
inode_ptr inode_state::get_root() { return root; }
inode_ptr inode_state::get_cwd() { return cwd; }

string inode_state::pwd() {
   inode_ptr curr = cwd;
   string pwd = "";
   // Loop finding file names until it hits root
   while(curr != root) {
      base_file_ptr dir = curr->get_contents();
      int inode_nr = dir->get_dirent(".")->get_inode_nr();
      curr = dir->get_dirent(".."); // Parent dir
      dir = curr->get_contents();
      pwd = dir->get_name(inode_nr) + pwd;
   }
   return "/" + pwd;
}

void inode_state::print_dir(inode_ptr dir, const wordvec& words, bool recur) {
   
}

void inode_state::set_prompt(const string& prompt) { 
  prompt_ = prompt;
}
void inode_state::set_root(inode_ptr newroot) { 
  root = newroot;
}
void inode_state::set_cwd(inode_ptr newcwd) { 
  cwd = newcwd;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

/**
 * Inode
 */

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   this->type = type;
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

base_file_ptr inode::get_contents() const {
   return contents;
}

bool inode::is_plain() {
   return type == file_type::PLAIN_TYPE;
}

bool inode::is_dir() { 
   return type == file_type::DIRECTORY_TYPE; 
}

/**
 * Files
 */


file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

wordvec plain_file::get_data() const {
   return data;
}

inode_ptr plain_file::get_dirent(string) const {
   throw file_error ("is a plain file");
}

string plain_file::get_name(int) const {
   throw file_error ("is a plain file");
}

/**
 * Directories
 */


size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

wordvec directory::get_data() const {
   throw file_error ("is a directory");
}

inode_ptr directory::get_dirent(string key) const {
   return dirents.at(key);
}

string directory::get_name(int inode_nr) const {
   for(auto itor = dirents.begin(); itor != dirents.end(); ++itor)
      if(itor->second->get_inode_nr() == inode_nr)
         return itor->first;
   return nullptr;
}

