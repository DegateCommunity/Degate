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

#include <ModuleWin.h>

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

ModuleWin::ModuleWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel) :
  GladeFileLoader("module_hierarchy.glade", "module_hierarchy_dialog") {

  this->lmodel = lmodel;
  this->parent = parent;
  assert(lmodel);

  if(pDialog) {

    // connect signals
    refXml->get_widget("close_button", close_button);
    if(close_button)
      close_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_close_button_clicked));
    
    
    refXml->get_widget("add_button", add_button);
    if(add_button) {
      add_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_add_button_clicked) );
      add_button->set_sensitive(false);
    }

    refXml->get_widget("remove_button", remove_button);
    if(remove_button) {
      remove_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_remove_button_clicked) );
      remove_button->set_sensitive(false);
    }

    refXml->get_widget("edit_button", edit_button);
    if(edit_button) {
      edit_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_edit_button_clicked) );
      edit_button->set_sensitive(false);
    }
    
    refXml->get_widget("goto_button", goto_button);
    if(goto_button) {
      goto_button->grab_focus();
      goto_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_goto_button_clicked) );
      remove_button->set_sensitive(false);
    }
    
    treemodel_modules = Gtk::TreeStore::create(columns_modules);
    refXml->get_widget("treeview_modules", treeview_modules);
    if(treeview_modules) {

      treeview_modules->set_model(treemodel_modules);
      treeview_modules->append_column("Module Name", columns_modules.m_col_name);
      treeview_modules->append_column("Module Type", columns_modules.m_col_type);

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = treeview_modules->get_selection();
      refTreeSelection->signal_changed().
	connect(sigc::mem_fun(*this, &ModuleWin::on_module_selection_changed));
    }    

    treemodel_gates = Gtk::TreeStore::create(columns_gates);
    refXml->get_widget("treeview_gates", treeview_gates);
    if(treeview_gates) {

      treeview_gates->set_model(treemodel_gates);
      treeview_gates->append_column("Gate Name", columns_gates.m_col_name);
      treeview_gates->append_column("Gate Type", columns_gates.m_col_type);

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = treeview_gates->get_selection();
      refTreeSelection->signal_changed().
	connect(sigc::mem_fun(*this, &ModuleWin::on_gate_selection_changed));
    }
  }

  insert_modules();

}

ModuleWin::~ModuleWin() {
}

void ModuleWin::show() {
  pDialog->show();
}



void ModuleWin::insert_modules() {

  treemodel_modules->clear();

  assert(lmodel != NULL);
  if(lmodel != NULL) {
    Module_shptr module = lmodel->get_main_module();
    assert(module != NULL); // must be present all the time

    Gtk::TreeModel::Row row = (*treemodel_modules->append());
    insert_module(row, module);
  }
}

void ModuleWin::insert_module(Gtk::TreeModel::Row & row, 
			      Module_shptr module) {

  row[columns_modules.m_col_name] = module->get_name();
  row[columns_modules.m_col_type] = module->get_entity_name();
  row[columns_modules.m_col_object_ptr] = module;


  for(Module::module_collection::iterator iter = module->modules_begin();
      iter != module->modules_end(); ++iter) {
    
    Module_shptr child_module = *iter;
    Gtk::TreeModel::Row child_row = *(treemodel_modules->append(row.children()));
    insert_module(row, child_module);
  }

}

void ModuleWin::insert_gates(Module_shptr module) {

  treemodel_gates->clear();

  for(Module::gate_collection::iterator iter = module->gates_begin();
      iter != module->gates_end(); ++iter) {
    
    Gate_shptr gate = *iter;

    Gtk::TreeModel::Row row = *(treemodel_gates->append());

    row[columns_gates.m_col_name] = gate->get_descriptive_identifier();
    row[columns_gates.m_col_type] =
      gate->has_template() ? gate->get_gate_template()->get_name() : "Gate";
    row[columns_gates.m_col_object_ptr] = gate;
  }
}

void ModuleWin::on_module_selection_changed() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Module_shptr module = row[columns_modules.m_col_object_ptr];

      remove_button->set_sensitive(true);
      edit_button->set_sensitive(true);
      add_button->set_sensitive(true);

      insert_gates(module);
    }
  }
  else {
    remove_button->set_sensitive(false);
    edit_button->set_sensitive(false);
    add_button->set_sensitive(false);
  }

}

void ModuleWin::on_gate_selection_changed() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_gates->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Gate_shptr gate = row[columns_gates.m_col_object_ptr];

      goto_button->set_sensitive(true);
    }
  }
  else 
    goto_button->set_sensitive(false);
}





void ModuleWin::on_close_button_clicked() {
  pDialog->hide();
}

void ModuleWin::on_add_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Module_shptr parent_module = row[columns_modules.m_col_object_ptr];
      assert(parent_module != NULL);

      Module_shptr new_mod(new Module("Click to edit"));
      parent_module->add_module(new_mod);

      Gtk::TreeModel::Row new_row = *(treemodel_modules->append(row.children()));
      insert_module(new_row, new_mod);
    }
  }
}

void ModuleWin::on_remove_button_clicked() {
}

void ModuleWin::on_edit_button_clicked() {
}


void ModuleWin::on_goto_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Gate_shptr gate = row[columns_gates.m_col_object_ptr];
      assert(gate != NULL);

      signal_goto_button_clicked_(gate);
    }
  }
}

sigc::signal<void, degate::PlacedLogicModelObject_shptr>& 
ModuleWin::signal_goto_button_clicked() {
  return signal_goto_button_clicked_;
}


void ModuleWin::object_removed(degate::Gate_shptr obj_ptr) {
}

