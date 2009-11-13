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


#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "Rectangle.h"
#include "TypeTraits.h"

#include <vector>
#include <list>
#include <assert.h>
#include "globals.h"
#include <iostream>

namespace degate {


  template<bool b> 
  struct get_bbox_trait_selector { 
    template<typename T>
    static BoundingBox const & get_bounding_box_for_object(T object) {
      return object.get_bounding_box();
    }
  }; 

  template <> 
  struct get_bbox_trait_selector<true> { 
    template<typename T>
    static BoundingBox const & get_bounding_box_for_object(T object) {
      return object->get_bounding_box();
    }
  }; 


  template <typename T> class QuadTree;
}

//#include "QuadTreeDownIterator.h"
#include "QuadTreeRegionIterator.h"

namespace degate {

  /**
   * Quad tree to store objects and to accesss them with a two dimensional access path.
   */
  template <typename T>
  class QuadTree {
  
    friend class region_iterator<T>;
    //    friend class down_iterator<T>;

  private:
    
    const static int NW = 0;
    const static int NE = 1;
    const static int SW = 2;
    const static int SE = 3;
    
    const static int bbox_min_size = 10;
    
    unsigned int  max_entries;
    
    BoundingBox box;
    std::vector<QuadTree<T> > subtree_nodes;
    std::list<T> children;
    
    QuadTree * parent;

    std::string node_name;

    QuadTree(BoundingBox const & box, QuadTree * parent, std::string node_name, int max_entries = 50);
    
    QuadTree<T> * traverse_downto_bounding_box(BoundingBox const & box);
    
    ret_t split();
    ret_t reinsert_objects();

    bool is_splitable() const;

  public:

    const std::string get_name() const { return node_name; }

    /**
     * Create a new quadtree.
     * @param box The bounding box defines the dimension of the quadtree.
     * @param max_entries Defines how many objects should be stored in a quadtree node, before
     *    the node is splitted. This value is not a hard limit. The quadtree can ignore this
     *    parameter, if it has no choice.
     */

    QuadTree(BoundingBox const & box, int max_entries = 50);

    /**
     * Destruct a quadtree.
     */

    ~QuadTree();
    
    /**
     * Check if the quadtree or a quadtree node is a leaf node.
     */

    bool is_leave() const;
    
    /**
     * Insert an object into the quadtree.
     */

    ret_t insert(T object);

    /**
     * Remove an object from the quadtree.
     */

    ret_t remove(T object);


    /**
     * Notify that the bounding box of an object changed.
     * It might be neccessary to adjust the objects position in the quadtree.
     */

    void notify_shape_change(T object);

    /**
     * Get the number of objects that are stored in a quadtree node and in all child nodes.     
     */

    unsigned int total_size() const;

    /**
     * Get the number of layers, that quadtree has.
     */

    unsigned int depth() const;

    /*
     * Check if there are objects stored in the quadtree.
     */

    bool is_empty() const { return total_size() == 0; }

    /**
     * Get the dimension of the quadtree.
     */

    int get_width() const;

    /**
     * Get the dimension of the quadtree.
     */

    int get_height() const;


    /**
     * Get an interator to iterate over quadtree nodes down to the leafes.
     */

    //    down_iterator<T> down_iter_begin();

    /**
     * Get an end marker for the iteration
     * @see down_iter_begin()
     */

    //    down_iterator<T> down_iter_end();

    /**
     * Get a region iterator to iterate over quatree objects.
     */


    region_iterator<T> region_iter_begin(int min_x, int max_x, int min_y, int max_y);

    /**
     * Get a region iterator to iterate over quatree objects.
     */

    region_iterator<T> region_iter_begin(BoundingBox const & bbox);

    /**
     * Get a region iterator to iterate over the complete quatree.
     */

    region_iterator<T> region_iter_begin();

    /**
     * Get an end marker for the region iteration.
     */

    region_iterator<T> region_iter_end();

    /**
     * Print the quadtree.
     */
    void print(std::ostream & os = std::cout, int tabs = 0, bool recursive = false);

  };




  template <typename T>
  QuadTree<T>::QuadTree(BoundingBox const & box, int max_entries) {
    this->box = box;
    this->max_entries = max_entries;
    parent = NULL;  
    node_name = "/";
  }
  
  template <typename T>
  QuadTree<T>::QuadTree(BoundingBox const & box, QuadTree * parent, std::string _node_name, int max_entries) {
    this->box = box;
    this->max_entries = max_entries;
    this->parent = parent;

    node_name = parent->node_name + std::string("/") + _node_name;
  }
  
  template <typename T>
  QuadTree<T>::~QuadTree() {
  }
  
  template <typename T>
  int QuadTree<T>::get_width() const {
    return box.get_width();
  }
  
  template <typename T>
  int QuadTree<T>::get_height() const {
    return box.get_height();
  }
  
  
  template <typename T>
  bool QuadTree<T>::is_splitable() const {
    return box.get_width() > bbox_min_size &&
      box.get_height() > bbox_min_size &&
      is_leave();
  }
  
  template <typename T>
  ret_t QuadTree<T>::split() {
    if(is_splitable()) {
      
      BoundingBox nw(box.get_min_x(), box.get_center_x(),
		     box.get_min_y(), box.get_center_y());
      
      
      BoundingBox sw(box.get_min_x(), box.get_center_x(),
		     box.get_center_y() + 1, box.get_max_y());
      
      
      BoundingBox ne(box.get_center_x() + 1, box.get_max_x(),
		     box.get_min_y(), box.get_center_y());
      
      
      BoundingBox se(box.get_center_x() + 1, box.get_max_x(),
		     box.get_center_y() + 1,  box.get_max_y());


      QuadTree<T> node_nw(nw, this, "NW", max_entries);
      QuadTree<T> node_ne(ne, this, "NE", max_entries);
      QuadTree<T> node_sw(sw, this, "SW", max_entries);
      QuadTree<T> node_se(se, this, "SE", max_entries);


      subtree_nodes.push_back(node_nw);
      subtree_nodes.push_back(node_ne);
      subtree_nodes.push_back(node_sw);
      subtree_nodes.push_back(node_se);

      return RET_OK;
    }

    debug(TM, "Failed to split a quadtree node of width %d and height %d that is %sa leave",
	  box.get_width(),
	  box.get_height(),
	  is_leave() ? "" : "not ");
    assert(1 == 0);
    return RET_ERR;
  }

  template <typename T>
  ret_t QuadTree<T>::reinsert_objects() {
  
    typename std::list<T> children_copy = children;
  
    children.clear();

    for(typename std::list<T>::iterator it = children_copy.begin();
	it != children_copy.end();
	++it) {
      insert(*it);
    }
  
    return RET_OK;
  }

  template <typename T>
  ret_t QuadTree<T>::insert(T object) {

    ret_t ret;
    const BoundingBox & bbox = 
      get_bbox_trait_selector<is_pointer<T>::value>::get_bounding_box_for_object(object);

    QuadTree<T> * found = traverse_downto_bounding_box(bbox);
    assert(found != NULL);

    if(found != NULL) {

      if((found->children.size() >= max_entries) && found->is_splitable()) {

	if(RET_IS_NOT_OK(ret = found->split())) return ret;
	if(RET_IS_NOT_OK(ret = found->reinsert_objects())) return ret;
	if(RET_IS_NOT_OK(ret = found->insert(object))) return ret;
	return RET_OK;
      }
      else {
	found->children.push_back(object);
	return RET_OK;
      }
    }
    return RET_ERR;
  }

  template <typename T>
  void QuadTree<T>::notify_shape_change(T object) {
    remove(object);
    insert(object);
  }

  template <typename T>
  ret_t QuadTree<T>::remove(T object) {
    const BoundingBox & bbox = 
      get_bbox_trait_selector<is_pointer<T>::value>::get_bounding_box_for_object(object);

    QuadTree<T> * found = traverse_downto_bounding_box(bbox);
    assert(found != NULL);
    if(found != NULL) {
      found->children.remove(object);

      if(!found->is_leave() &&
	 found->subtree_nodes[NW].children.size() == 0 &&
	 found->subtree_nodes[NE].children.size() == 0 &&
	 found->subtree_nodes[SW].children.size() == 0 &&
	 found->subtree_nodes[SE].children.size() == 0) {
	found->subtree_nodes.clear();
      }

      return RET_OK;
    }
    return RET_ERR;
  }


  template <typename T>
  bool QuadTree<T>::is_leave() const {
    return subtree_nodes.size() == 4 ? false : true;
  }

  template <typename T>
  QuadTree<T> * QuadTree<T>::traverse_downto_bounding_box(BoundingBox const & box) {

    if(is_leave()) return this;

    for(typename std::vector< QuadTree<T> >::iterator it = subtree_nodes.begin();
	it != subtree_nodes.end();
	++it) {

      const BoundingBox & sub_bbox = (*it).box.get_bounding_box();

      //if(sub_bbox.in_bounding_box(box)) { // sub_bbox within box?
	if(box.in_bounding_box(sub_bbox)) { // box within sub_bbox?
	return (*it).traverse_downto_bounding_box(box);
      }
    }
    //    assert(1 == 0);
    return this;
  }

  template <typename T>
  unsigned int QuadTree<T>::total_size() const {
    unsigned int this_node = children.size();
    unsigned int sub_nodes = 0;
    if(!is_leave()) {
      for(typename std::vector< QuadTree<T> >::const_iterator it = subtree_nodes.begin();
	  it != subtree_nodes.end();
	  ++it) {
	sub_nodes += (*it).total_size();
      }
    }
    return this_node + sub_nodes;
  }

  template <typename T>
  unsigned int QuadTree<T>::depth() const {

    unsigned int max_d = 0;
    if(!is_leave()) {
      for(typename std::vector< QuadTree<T> >::const_iterator it = subtree_nodes.begin();
	  it != subtree_nodes.end();
	  ++it) {
      
	unsigned int d = (*it).depth();
	max_d = std::max(max_d, d);
      }
    }
    return 1 + max_d;
  }


  /*
  template <typename T>
  down_iterator<T> QuadTree<T>::down_iter_begin() {
    return down_iterator<T>(this);
  }

  template <typename T>
  down_iterator<T> QuadTree<T>::down_iter_end() {
    return down_iterator<T>();
  }
  */

  template <typename T>
  region_iterator<T> QuadTree<T>::region_iter_begin(int min_x, int max_x, int min_y, int max_y) {

    BoundingBox bbox(min_x, max_x, min_y, max_y);
    return region_iter_begin(bbox);
  }

  template <typename T>
  region_iterator<T> QuadTree<T>::region_iter_begin(BoundingBox const & bbox) {
    return region_iterator<T>(this, bbox);
  }

  template <typename T>
  region_iterator<T> QuadTree<T>::region_iter_begin() {
    return region_iterator<T>(this, box);
  }

  template <typename T>
  region_iterator<T> QuadTree<T>::region_iter_end() {
    return region_iterator<T>();
  }



  template <typename T>
  void QuadTree<T>::print(std::ostream & os, int tabs, bool recursive) {

    os 
      << gen_tabs(tabs) << "Node name                      : " << get_name() << std::endl
      << gen_tabs(tabs) << "Bounding box                   : x = " 
      << box.get_min_x() << " .. " << box.get_max_x()
      << " / y = " 
      << box.get_min_y() << " .. " << box.get_max_y() 
      << std::endl

      << gen_tabs(tabs) << "Num elements in this node      : " << children.size() << std::endl
      << gen_tabs(tabs) << "Preferred max num of elements : " << max_entries << std::endl
      << std::endl
      ;

    if(parent == NULL /* || children.size() < 5 */ ) {

      for(typename std::list<T>::iterator c_iter = children.begin(); 
	  c_iter != children.end(); ++c_iter) {

	const BoundingBox & e_bb = 
	  get_bbox_trait_selector<is_pointer<T>::value>::get_bounding_box_for_object(*c_iter);

	os
	  << gen_tabs(tabs) << "    + Element bounding box           : x = " 
	  << e_bb.get_min_x() << " .. " << e_bb.get_max_x()
	  << " / y = " 
	  << e_bb.get_min_y() << " .. " << e_bb.get_max_y() 
	  << std::endl;
      }

      os << std::endl;
    }

    if(recursive) {
      for(typename std::vector<QuadTree<T> >::iterator stn_iter = subtree_nodes.begin();
	  stn_iter != subtree_nodes.end(); ++stn_iter) {
	(*stn_iter).print(os, tabs+1);
      }
    }

    
  }

  /* missing:
     - get object(s) at
     - find object (?)
  */

}

#endif
