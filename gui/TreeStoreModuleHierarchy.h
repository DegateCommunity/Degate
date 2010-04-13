/* -*-c++-*-
 
  This file is part of the IC reverse engineering tool degate.
 
  Copyright 2008, 2009, 2010 by Martin Schobert
 
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

#ifndef __TREESTOREMODULEHIERARCHY_H__
#define __TREESTOREMODULEHIERARCHY_H__

#include <degate.h>
#include <gtkmm.h>

class TreeStoreModuleHierarchy : public Gtk::TreeStore {

 private: 
  TreeStoreModuleHierarchy() {
    set_column_types(m_columns);
  }

 public:

  class ModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    ModelColumns() {
      add(m_col_name);
      add(m_col_type);
      add(m_col_object_ptr);
      add(m_col_orig_parent_ptr);
    }
    
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_type;
    Gtk::TreeModelColumn<degate::Module_shptr> m_col_object_ptr;
    Gtk::TreeModelColumn<degate::Module_shptr> m_col_orig_parent_ptr;
  };

  ModelColumns m_columns;

 public:

  static Glib::RefPtr<TreeStoreModuleHierarchy> create() {
    return Glib::RefPtr<TreeStoreModuleHierarchy>(new TreeStoreModuleHierarchy()); 
  }

 private:


  bool row_draggable_vfunc(const Gtk::TreeModel::Path& path) const {
      Gtk::TreeModel::Path parent = path;
      bool path_is_not_top_level = parent.up();
      std::cout << (path_is_not_top_level ? "yes" : "no") << std::endl;
      return path_is_not_top_level;
  }


  bool row_drop_possible_vfunc(const Gtk::TreeModel::Path& dest, 
			       const Gtk::SelectionData& selection_data) const {
      Gtk::TreeModel::Path dest_parent = dest;
      bool dest_is_not_top_level = dest_parent.up();
      if(!dest_is_not_top_level || dest_parent.empty()) {
	return false;
      }
      return true;
  }

};


#endif
