// $Id: listmap.tcc,v 1.3 2016-02-17 06:28:56-08 - - $
// James Garbagnati
// jgarbagn
// 1354722

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   TRACE ('l', (void*) this);

   while(begin() != end())   
      erase(begin());
}


//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   TRACE ('l', &pair << "->" << pair);
   
   // Obtain position to insert
   iterator itor = begin();
   for(; itor != end(); ++itor) {
      // found where to insert
      if(!less(itor->first, pair.first)) {
         // keys match -> replace
         if(!less(pair.first, itor->first)) {
            itor->second = pair.second;
            return itor;
         }
         break;
      }
   }
   
   // Insert new node
   node *new_node = new node(itor.where, itor.where->prev, pair);
   if(itor.where->prev != nullptr) // yolosafetyfirst
      itor.where->prev->next = new_node;
   itor.where->prev = new_node;
   
   return iterator(new_node);
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that);
   
   // If found, return, otherwise exit and return the end
   iterator itor = begin();
   for(; itor != end(); ++itor) {
      if(!less(itor->first, that)) {
         if(!less(that, itor->first)) {
            return itor;
         }
         break;
      }
   }
   
   return end();
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   TRACE ('l', &*position);
   
   iterator next = iterator(position.where->next);
   position.erase();
   
   return next;
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

// 
// void listmap::iterator::erase()
// 
template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::iterator::erase() {
   if(where == nullptr)
      return;
   if(where->prev != nullptr) // Avoid errors
      where->prev->next = where->next;
   if(where->next != nullptr) // Avoid errors
      where->next->prev = where->prev;
   delete where;
}
