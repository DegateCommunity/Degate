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

#ifndef __SELECTMODULEWIN_H__
#define __SELECTMODULEWIN_H__

#include <degate.h>
#include <LogicModelObjectBase.h>
#include "GladeFileLoader.h"
#include <gtkmm.h>

#include <list>
#include <TreeStoreModuleHierarchy.h>

class SelectModuleWin : public Gtk::Window, private GladeFileLoader {


 public:

  SelectModuleWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel);

  virtual ~SelectModuleWin();

  /**
   * Show the dialog
   * @return Returns a shared pointer to the selected module. The method
   *   returns a NULL pointer equivalent, if the user clicks on the cancel
   *   button.
   */
  degate::Module_shptr show();

 private:


  Gtk::Window *parent;
  degate::LogicModel_shptr lmodel;

  degate::Module_shptr selected_module;

  Gtk::Button* add_button;
  Gtk::Button* remove_button;
  Gtk::Button* ok_button;
  Gtk::Button* cancel_button;


  Glib::RefPtr<TreeStoreModuleHierarchy> treemodel_modules;
  Gtk::TreeView* treeview_modules;

  void update_logic_model(Gtk::TreeModel::Children const& children,
			  degate::Module_shptr parent_module);

  void insert_modules();
  void insert_module(Gtk::TreeModel::Row & row, 
		     degate::Module_shptr module,
		     degate::Module_shptr parent_module);


  void insert_gates(degate::Module_shptr module);

  // Signal handlers:
  virtual void on_cancel_button_clicked();
  virtual void on_ok_button_clicked();

  virtual void on_add_button_clicked();
  virtual void on_remove_button_clicked();

  virtual void on_module_selection_changed();

};

#endif
