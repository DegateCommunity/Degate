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

#include <SelectModuleWin.h>

#include <gdkmm/window.h>
#include <gtkmm/stock.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <set>

#include <globals.h>
#include <boost/foreach.hpp>

using namespace degate;

SelectModuleWin::SelectModuleWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel) :
  GladeFileLoader("select_module.glade", "select_module_dialog") {

  this->parent = parent;
  this->lmodel = lmodel;
  assert(parent);
  assert(lmodel);

  if(pDialog) {

    // connect signals
    refXml->get_widget("cancel_button", cancel_button);
    assert(cancel_button != NULL);
    if(cancel_button)
      cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &SelectModuleWin::on_cancel_button_clicked));
    
    refXml->get_widget("ok_button", ok_button);
    assert(ok_button != NULL);
    if(ok_button) {
      ok_button->signal_clicked().connect(sigc::mem_fun(*this, &SelectModuleWin::on_ok_button_clicked) );
      ok_button->set_sensitive(false);
    }
    
    refXml->get_widget("add_button", add_button);
    assert(add_button != NULL);
    if(add_button) {
      add_button->signal_clicked().connect(sigc::mem_fun(*this, &SelectModuleWin::on_add_button_clicked) );
      add_button->set_sensitive(false);
    }

    refXml->get_widget("remove_button", remove_button);
    assert(remove_button != NULL);
    if(remove_button) {
      remove_button->signal_clicked().connect(sigc::mem_fun(*this, &SelectModuleWin::on_remove_button_clicked) );
      remove_button->set_sensitive(false);
    }

       
    treemodel_modules = TreeStoreModuleHierarchy::create();
    refXml->get_widget("treeview_modules", treeview_modules);
    assert(treeview_modules != NULL);
    if(treeview_modules) {

      treeview_modules->set_model(treemodel_modules);
      treeview_modules->append_column_editable("Module Name", treemodel_modules->m_columns.m_col_name);
      treeview_modules->append_column_editable("Module Type", treemodel_modules->m_columns.m_col_type);

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = treeview_modules->get_selection();
      refTreeSelection->signal_changed().
	connect(sigc::mem_fun(*this, &SelectModuleWin::on_module_selection_changed));


      treeview_modules->set_reorderable();

    }    

  }

  insert_modules();
}

SelectModuleWin::~SelectModuleWin() {
}


void SelectModuleWin::update_logic_model(Gtk::TreeModel::Children const& children,
					 Module_shptr parent_module) {
  
  for(Gtk::TreeModel::Children::iterator iter = children.begin();
      iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;

    Module_shptr module = row[treemodel_modules->m_columns.m_col_object_ptr];
    Module_shptr orig_parent = row[treemodel_modules->m_columns.m_col_orig_parent_ptr];
    Glib::ustring name = row[treemodel_modules->m_columns.m_col_name];
    Glib::ustring type = row[treemodel_modules->m_columns.m_col_type];

    std::cout << "name=" << module->get_name() << " - " << name << " type=" << type << std::endl;

    assert(module != NULL);
    module->set_name(name);
    module->set_entity_name(type);

    if(parent_module != NULL && orig_parent != parent_module) {
      orig_parent->remove_module(module);
      parent_module->add_module(module);
    }

    update_logic_model(row.children(), module);
  }
}


degate::Module_shptr SelectModuleWin::show() {
  pDialog->run();
  update_logic_model(treemodel_modules->children(), Module_shptr());
  return selected_module;
}



void SelectModuleWin::insert_modules() {

  treemodel_modules->clear();

  assert(lmodel != NULL);
  if(lmodel != NULL) {
    Module_shptr module = lmodel->get_main_module();
    assert(module != NULL); // must be present all the time

    Gtk::TreeModel::Row row = (*treemodel_modules->append());
    insert_module(row, module, Module_shptr());
  }
}

void SelectModuleWin::insert_module(Gtk::TreeModel::Row & row, 
				    Module_shptr module,
				    Module_shptr parent_module) {

  row[treemodel_modules->m_columns.m_col_name] = module->get_name();
  row[treemodel_modules->m_columns.m_col_type] = module->get_entity_name();
  row[treemodel_modules->m_columns.m_col_object_ptr] = module;
  row[treemodel_modules->m_columns.m_col_orig_parent_ptr] = parent_module;


  for(Module::module_collection::iterator iter = module->modules_begin();
      iter != module->modules_end(); ++iter) {
    
    Module_shptr child_module = *iter;
    Gtk::TreeModel::Row child_row = *(treemodel_modules->append(row.children()));
    insert_module(child_row, child_module, module);
  }

}


void SelectModuleWin::on_module_selection_changed() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      selected_module = row[treemodel_modules->m_columns.m_col_object_ptr];

      remove_button->set_sensitive(true);
      add_button->set_sensitive(true);
      ok_button->set_sensitive(true);
    }
  }
  else {
    remove_button->set_sensitive(false);
    add_button->set_sensitive(false);
    ok_button->set_sensitive(false);
  }

}

void SelectModuleWin::on_cancel_button_clicked() {
  pDialog->hide();
  selected_module.reset();
}

void SelectModuleWin::on_ok_button_clicked() {
  pDialog->hide();
}

void SelectModuleWin::on_add_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Module_shptr parent_module = row[treemodel_modules->m_columns.m_col_object_ptr];
      assert(parent_module != NULL);

      Module_shptr new_mod(new Module("Click to edit"));
      std::cout << "add module " << new_mod->get_name() << " to module " << parent_module->get_name() << std::endl;
      parent_module->add_module(new_mod);

      Gtk::TreeModel::Row new_row = *(treemodel_modules->append(row.children()));
      insert_module(new_row, new_mod, parent_module);
    }
  }
}

void SelectModuleWin::on_remove_button_clicked() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {

      Gtk::MessageDialog dialog(*this, "Do you really want to remove the selected module?",
                                true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("Warning");
      if(dialog.run() == Gtk::RESPONSE_YES) {

	Module_shptr root_module = lmodel->get_main_module();
	assert(root_module != NULL);
	if(!root_module->remove_module(selected_module)) {
	  Gtk::MessageDialog dialog(*parent, "Can't remove module.", true, Gtk::MESSAGE_ERROR);
	  dialog.set_title("Error");
	  dialog.run();	
	}
	else
	  treemodel_modules->erase(iter);
      }

    }
  }
}



