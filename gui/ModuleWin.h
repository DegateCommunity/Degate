/*                                                                              
                                                                                
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

#ifndef __MODULEWIN_H__
#define __MODULEWIN_H__

#include <degate.h>
#include <LogicModelObjectBase.h>
#include "GladeFileLoader.h"
#include <gtkmm.h>
#include <TreeStoreModuleHierarchy.h>
#include <list>

class ModuleWin : public Gtk::Window, private GladeFileLoader {

  class GatesModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    GatesModelColumns() {
      add(m_col_name);
      add(m_col_type);
      add(m_col_object_ptr);
      add(m_col_parent_ptr);
    }
    
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_type;
    Gtk::TreeModelColumn<degate::Gate_shptr> m_col_object_ptr;
    Gtk::TreeModelColumn<degate::Module_shptr> m_col_parent_ptr;
  };

  class PortsModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    PortsModelColumns() {
      add(m_col_name);
      add(m_col_gate_port);
      add(m_col_gate);
      add(m_col_object_ptr);
    }
    
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_gate_port;
    Gtk::TreeModelColumn<Glib::ustring> m_col_gate;
    Gtk::TreeModelColumn<degate::Gate_shptr> m_col_object_ptr;
  };

 public:

  ModuleWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel);

  virtual ~ModuleWin();

  /**
   * Display the window.
   */
  void show();

  
  /**
   * Indicate, that a logic model object is or will
   * be removed and that the object should be removed from the inspection.
   */
  void update();

  /**
   * Set up a callback mechanism for the case a user wants
   * to jump to a logic model object.
   */
  sigc::signal<void, degate::PlacedLogicModelObject_shptr>& signal_goto_button_clicked();

 private:


  Gtk::Window *parent;
  degate::LogicModel_shptr lmodel;

  Gtk::Button* add_button;
  Gtk::Button* remove_button;
  Gtk::Button* edit_button;
  Gtk::Button* goto_button;
  Gtk::Button* close_button;

  Glib::RefPtr<TreeStoreModuleHierarchy> treemodel_modules;
  Gtk::TreeView* treeview_modules;

  GatesModelColumns columns_gates;
  Glib::RefPtr<Gtk::TreeStore> treemodel_gates;
  Gtk::TreeView * treeview_gates;

  PortsModelColumns columns_ports;
  Glib::RefPtr<Gtk::TreeStore> treemodel_ports;
  Gtk::TreeView * treeview_ports;


  sigc::signal<void, degate::PlacedLogicModelObject_shptr> signal_goto_button_clicked_;

  void insert_modules();
  void insert_module(Gtk::TreeModel::Row & row, 
		     degate::Module_shptr module,
		     degate::Module_shptr parent_module);


  void insert_gates(degate::Module_shptr module);
  void insert_ports(degate::Module_shptr module);

  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual void on_goto_button_clicked();

  virtual void on_add_button_clicked();
  virtual void on_remove_button_clicked();

  virtual void on_edit_button_clicked();

  virtual void on_module_selection_changed();
  virtual void on_gate_selection_changed();
  virtual void on_port_selection_changed();

  void update_logic_model(Gtk::TreeModel::Children const& children,
			  degate::Module_shptr parent_module);


};

#endif
