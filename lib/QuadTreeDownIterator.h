/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
 Degate is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 Degate is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with degate. If not, see <http://www.gnu.org/licenses/>.
 
*/

#ifndef __QUADTREEDOWNITERATOR_H__
#define	__QUADTREEDOWNITERATOR_H__

#include "QuadTree.h"

namespace degate {

template<typename T>
class down_iterator : public std::iterator<std::forward_iterator_tag, T> {

 private:
  QuadTree<T> * node;
  bool done;
  
  typename std::list<T>::iterator children_iter;
  typename std::list<T>::iterator children_iter_end;
  
  std::list<QuadTree<T> *> open_list;
  
  void next_node();
  
 public:
  down_iterator();
  down_iterator(QuadTree<T> * node);
  virtual ~down_iterator() {}
  virtual down_iterator& operator=(const down_iterator& other);
  virtual down_iterator& operator++();
  virtual bool operator==(const down_iterator& other) const;
  virtual bool operator!=(const down_iterator& other) const;
  virtual T * operator->() const;
  virtual T operator*() const;
};


template<typename T>
down_iterator<T>::down_iterator() : node(NULL), done(true) {
}

template<typename T>
down_iterator<T>::down_iterator(QuadTree<T> * _node) : 
  node(NULL),
  done(false) {

  assert(_node != NULL);

  open_list.push_back(_node);
  next_node();
}

template<typename T>
void down_iterator<T>::next_node() {
  if(node == NULL) {
    if(open_list.size() > 0) {
      node = open_list.front();
      open_list.pop_front();

      // add subtree nodes to open list
      for(typename std::vector<QuadTree<T> >::iterator it = node->subtree_nodes.begin();
	  it != node->subtree_nodes.end();
	  ++it)
	open_list.push_back(&*it);

      // reset iterator for current
      children_iter = node->children.begin();
      children_iter_end = node->children.end();

      if(children_iter == children_iter_end) {
	done = true;
      }

    }
    else {
      done = true;
    }
  }

}

template<typename T>
down_iterator<T>& down_iterator<T>::operator++() {

  if(!done) {
    ++children_iter;
  }

  if(!done && children_iter == children_iter_end) {
    node = NULL;
    next_node();
  }


  return (*this);
}

template<typename T>
down_iterator<T>& down_iterator<T>::operator=(const down_iterator& other) {
  node = other.node;
  done = other.done;
  open_list = other.open_list;
  children_iter = other.children_iter;
  children_iter_end = other.children_iter_end;
  return(*this);
}

template<typename T>
bool down_iterator<T>::operator==(const down_iterator& other) const {

  if(done == true && other.done == true)
    return true;
  else
    return (node == other.node &&
	    children_iter == other.children_iter &&
	    open_list == other.open_list &&
	    done == other.done);
}


template<typename T>
bool down_iterator<T>::operator!=(const down_iterator& other) const {
  return !(*this == other);
}

template<typename T>
T * down_iterator<T>::operator->() const {
  return &*children_iter;
}

template<typename T>
T down_iterator<T>::operator*() const {
  return *children_iter;
}

}

#endif

