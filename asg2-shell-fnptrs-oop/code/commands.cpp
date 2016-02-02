// $Id: commands.cpp,v 1.1 2016-01-27 00:02:51-08 - - $
// James Garbagnati
// jgarbagn
// 1354722


#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"#"     , fn_cmnt  },
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cmnt (inode_state& state, const wordvec& words){
   // Commented, yo.
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cat (inode_state& state, const wordvec& words){
   if(words.size() == 1)
      throw command_error("cat: no file");
   
   for(auto itor = words.begin() + 1; itor != words.end(); ++itor) {
      try {
         inode_ptr node = state.find(*itor, 0);
         base_file_ptr file = node->get_contents();
         if(file != nullptr) {
            if(node->is_plain()) {
               wordvec content(file->readfile());
               if(content.size() == 1) {
                  cout << content.at(0);
               } else if(content.size() > 1) {
                  string last_word = content.back();
                  content.pop_back();
                  for(auto const& word: content) {
                     cout << word << " ";
                  }
                  cout << last_word;
               }
               cout << endl;
            } else {
               throw command_error("cat: directory given");
            }
         } else {
            throw command_error("cat: path invalid");
         }
      } catch(file_error& error) {
         complain() << error.what() << endl;
      }
   }
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   if(words.size() > 2)
      throw command_error("cd: too many arguments");
   
   try{
      if(words.size() == 1) {
         state.cd("/");
      } else {
         state.cd(words.at(1));
      }
   } catch (file_error& error) {
      complain() << error.what() << endl;
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void ls (inode_state& state, const wordvec& words, bool recur) {
   if(words.size() == 1) {
      // Print current directory:
      state.ls(state, state.get_cwd(), recur);
   } else {
      // Print specified directory:
      for(auto itor = words.begin() + 1; 
             itor != words.end(); ++itor) {
         try {
            inode_ptr node = state.find(*itor, 0);
            if(node != nullptr) {
               if(node->is_dir())
                  state.ls(state, node, recur);
               else
                  throw command_error("ls: file given");
            } else {
               throw command_error("ls: path invalid");
            }
         } catch (file_error& error) {
            complain() << error.what() << endl;
         }
      }
   }
}

void fn_ls (inode_state& state, const wordvec& words){
   ls(state, words, false);
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   ls(state, words, true);
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   if(words.size() == 1)
      throw command_error("make: too few arguments");
   
   try {
      // Make File
      inode_ptr node = state.find(words.at(1), 1);
      wordvec path = split(words.at(1), "/");
      base_file_ptr dir = node->get_contents();
      node = dir->mkfile(path.back());
      
      // Write File
      base_file_ptr file = node->get_contents();
      wordvec content(words);
      content.erase(content.begin(), content.begin()+2);
      file->writefile(content);
   } catch (file_error& error) {
      complain() << error.what() << endl;
   }
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   if(words.size() == 1)
      throw command_error("mkdir: no directory name");
   if(words.size() > 2)
      throw command_error("mkdir: too many argumentss");
   
   try {
      inode_ptr node = state.find(words.at(1), 1);
      wordvec path = split(words.at(1), "/");
      base_file_ptr dir = node->get_contents();
      dir->mkdir(path.back());
   } catch (file_error& error) {
      complain() << error.what() << endl;
   }
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   if(words.size() == 1)
      throw command_error("prompt: too few arguments");

   string prompt = "";
   for(auto itor = words.begin() + 1;
         itor != words.end(); ++itor)
      prompt += *itor + " ";
   state.set_prompt(prompt);

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   cout << state.pwd(state.get_cwd()) << endl;

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

