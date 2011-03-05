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

#include <VerilogModuleGenerator.h>

#include <ModuleWin.h>
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

ModuleWin::ModuleWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel) :
  GladeFileLoader("module_hierarchy.glade", "module_hierarchy_dialog") {

  this->lmodel = lmodel;
  this->parent = parent;
  assert(lmodel);

  if(get_dialog()) {

    // connect signals
    get_widget("close_button", close_button);
    if(close_button)
      close_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_close_button_clicked));


    get_widget("add_button", add_button);
    if(add_button) {
      add_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_add_button_clicked) );
      add_button->set_sensitive(false);
    }

    get_widget("remove_button", remove_button);
    if(remove_button) {
      remove_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_remove_button_clicked) );
      remove_button->set_sensitive(false);
    }

    get_widget("move_button", move_button);
    if(move_button) {
      move_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_move_button_clicked) );
      move_button->set_sensitive(false);
    }

    get_widget("determine_ports_button", determine_ports_button);
    if(determine_ports_button) {
      determine_ports_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_determine_ports_button_clicked) );
      determine_ports_button->set_sensitive(false);
    }

    get_widget("export_button", export_button);
    if(export_button) {
      export_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_export_button_clicked) );
      export_button->set_sensitive(false);
    }


    get_widget("goto_button", goto_button);
    if(goto_button) {
      goto_button->grab_focus();
      goto_button->signal_clicked().connect(sigc::mem_fun(*this, &ModuleWin::on_goto_button_clicked) );
      goto_button->set_sensitive(false);
    }

    treemodel_modules = TreeStoreModuleHierarchy::create();
    get_widget("treeview_modules", treeview_modules);
    if(treeview_modules) {

      treeview_modules->set_model(treemodel_modules);
      treeview_modules->append_column_editable("Module Name", treemodel_modules->m_columns.m_col_name);
      treeview_modules->append_column_editable("Module Type", treemodel_modules->m_columns.m_col_type);

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = treeview_modules->get_selection();
      refTreeSelection->signal_changed().
	connect(sigc::mem_fun(*this, &ModuleWin::on_module_selection_changed));


      Gtk::CellRendererText * rendererText;

      rendererText = dynamic_cast<Gtk::CellRendererText *>(treeview_modules->get_column_cell_renderer(0)); 
      rendererText->signal_edited().connect(sigc::mem_fun(*this, &ModuleWin::on_module_name_edited));
      rendererText = dynamic_cast<Gtk::CellRendererText *>(treeview_modules->get_column_cell_renderer(1)); 
      rendererText->signal_edited().connect(sigc::mem_fun(*this, &ModuleWin::on_module_type_edited));

    }

    treemodel_gates = Gtk::TreeStore::create(columns_gates);
    get_widget("treeview_gates", treeview_gates);
    if(treeview_gates) {

      treeview_gates->set_model(treemodel_gates);
      treeview_gates->append_column("Gate Name", columns_gates.m_col_name);
      treeview_gates->append_column("Gate Type", columns_gates.m_col_type);

      Gtk::TreeView::Column * pColumn;

      pColumn = treeview_gates->get_column(0);
      if(pColumn) pColumn->set_sort_column(columns_gates.m_col_name);

      pColumn = treeview_gates->get_column(1);
      if(pColumn) pColumn->set_sort_column(columns_gates.m_col_type);

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = treeview_gates->get_selection();
      refTreeSelection->signal_changed().
	connect(sigc::mem_fun(*this, &ModuleWin::on_gate_selection_changed));
    }


    treemodel_ports = Gtk::TreeStore::create(columns_ports);
    get_widget("treeview_ports", treeview_ports);
    if(treeview_ports) {

      treeview_ports->set_model(treemodel_ports);
      treeview_ports->append_column_editable("Module Port Name", columns_ports.m_col_name);
      treeview_ports->append_column("Gate Port", columns_ports.m_col_gate_port);
      treeview_ports->append_column("Gate", columns_ports.m_col_gate);

      Gtk::TreeView::Column * pColumn;

      pColumn = treeview_gates->get_column(0);
      if(pColumn) pColumn->set_sort_column(columns_ports.m_col_name);

      pColumn = treeview_gates->get_column(1);
      if(pColumn) pColumn->set_sort_column(columns_ports.m_col_gate_port);

      pColumn = treeview_gates->get_column(2);
      if(pColumn) pColumn->set_sort_column(columns_ports.m_col_gate);

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = treeview_ports->get_selection();
      refTreeSelection->signal_changed().
	connect(sigc::mem_fun(*this, &ModuleWin::on_port_selection_changed));
    }
  }

  insert_modules();

}

ModuleWin::~ModuleWin() {
}

void ModuleWin::show() {
  get_dialog()->show();
}



void ModuleWin::insert_modules() {

  treemodel_modules->clear();

  assert(lmodel != NULL);
  if(lmodel != NULL) {
    Module_shptr module = lmodel->get_main_module();
    assert(module != NULL); // must be present all the time

    Gtk::TreeModel::Row row = (*treemodel_modules->append());
    insert_module(row, module, Module_shptr());
  }
}

void ModuleWin::insert_module(Gtk::TreeModel::Row & row,
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

void ModuleWin::insert_ports(Module_shptr module) {

  treemodel_ports->clear();
  for(Module::port_collection::const_iterator iter = module->ports_begin();
      iter != module->ports_end(); ++iter) {

    std::string mod_port_name = iter->first;

    BOOST_FOREACH(GatePort_shptr gp, iter->second) {
      Gtk::TreeModel::Row row = *(treemodel_ports->append());

      Gate_shptr gate = gp->get_gate();
      assert(gate != NULL);

      row[columns_ports.m_col_name] = mod_port_name;
      row[columns_ports.m_col_gate_port] = gp->get_descriptive_identifier();
      row[columns_ports.m_col_gate] = gate->get_descriptive_identifier();
      row[columns_ports.m_col_object_ptr] = gate;
    }
  }

}

void ModuleWin::on_module_name_edited(const Glib::ustring& path, const Glib::ustring& new_text) {
  if(Module_shptr mod = get_selected_module())
    mod->set_name(new_text);
}

void ModuleWin::on_module_type_edited(const Glib::ustring& path, const Glib::ustring& new_text) {
  if(Module_shptr mod = get_selected_module())
    mod->set_entity_name(new_text);
}

void ModuleWin::on_module_selection_changed() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Module_shptr module = row[treemodel_modules->m_columns.m_col_object_ptr];

      remove_button->set_sensitive(!module->is_main_module());
      add_button->set_sensitive(true);
      export_button->set_sensitive(true);
      determine_ports_button->set_sensitive(module->is_main_module());

      insert_gates(module);
      insert_ports(module);
    }
    else {
      remove_button->set_sensitive(false);
      add_button->set_sensitive(false);
      export_button->set_sensitive(false);
      determine_ports_button->set_sensitive(false);
    }
  }

  on_gate_selection_changed(); // must be unselected
}

void ModuleWin::on_gate_selection_changed() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_gates->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Gate_shptr gate = row[columns_gates.m_col_object_ptr];

      goto_button->set_sensitive(true);
      move_button->set_sensitive(true);
    }
    else {
      goto_button->set_sensitive(false);
      move_button->set_sensitive(false);
    }
  }
}


void ModuleWin::on_port_selection_changed() {
}


void ModuleWin::on_close_button_clicked() {
  get_dialog()->hide();
  update_logic_model(treemodel_modules->children(), Module_shptr());
}

void ModuleWin::on_add_button_clicked() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;      
      Module_shptr parent_module = row[treemodel_modules->m_columns.m_col_object_ptr];


      Module_shptr new_mod(new Module("Click to edit"));
      std::cout << "add module " << new_mod->get_name() << " to module " << parent_module->get_name() << std::endl;
      parent_module->add_module(new_mod);
    
      Gtk::TreeModel::Row new_row = *(treemodel_modules->append(row.children()));
      insert_module(new_row, new_mod, parent_module);
    }
  }
}


Module_shptr ModuleWin::get_selected_module() {
  Module_shptr mod;

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;      
      mod = row[treemodel_modules->m_columns.m_col_object_ptr];
    }
  }
  return mod;
}

void ModuleWin::on_remove_button_clicked() {


  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_modules->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;      
      Module_shptr mod = row[treemodel_modules->m_columns.m_col_object_ptr];

      Module_shptr root_module = lmodel->get_main_module();
      assert(root_module != NULL);
      
      if(root_module == mod) return; // can't remove root module

      Gtk::MessageDialog dialog(*this, "Do you really want to remove the selected module?",
				true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("Warning");
      if(dialog.run() == Gtk::RESPONSE_YES) {
      

	if(!root_module->remove_module(mod)) {
	  Gtk::MessageDialog dialog(*parent, "Can't remove module.", true, Gtk::MESSAGE_ERROR);
	  dialog.set_title("Error");
	  dialog.run();
	}
	else {
	  treemodel_modules->erase(iter);
	  on_module_selection_changed();

	  treemodel_gates->clear();
	  treemodel_ports->clear();
	  
	}
      }
    }
  }
}


void ModuleWin::on_export_button_clicked() {
  Module_shptr mod = get_selected_module();

  if(mod) {

    // create a new generator
    VerilogModuleGenerator codegen(mod);

    // set module ports
    for(Module::port_collection::const_iterator iter = mod->ports_begin();
	iter != mod->ports_end(); ++iter) {

      const std::string mod_port_name = iter->first;
      const GatePort_shptr gp = iter->second.front();
      const GateTemplatePort_shptr tmpl_port = gp->get_template_port();
      assert(tmpl_port != NULL);
      codegen.add_port(mod_port_name, tmpl_port->is_inport());
    }

    std::string impl = codegen.generate();

    std::cout << "Module-Code:\n" << impl << std::endl;
  }
}

void ModuleWin::on_determine_ports_button_clicked() {

  debug(TM, "should determine module ports");

  Module_shptr mod = get_selected_module();

  if(mod) {
    debug(TM, "have module");
    if(mod->is_main_module()) {

      debug(TM, "is main module");

      determine_module_ports_for_root(lmodel);
      insert_ports(mod);
    }
  }
}


void ModuleWin::on_goto_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_gates->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Gate_shptr gate = row[columns_gates.m_col_object_ptr];
      assert(gate != NULL);

      if(!signal_goto_button_clicked_.empty()) signal_goto_button_clicked_(gate);
    }
  } //
}


void ModuleWin::on_move_button_clicked() {

  Module_shptr selected_mod = get_selected_module();

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  treeview_gates->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Gate_shptr gate = row[columns_gates.m_col_object_ptr];
      assert(gate != NULL);

      SelectModuleWin smWin(this, lmodel);
      Module_shptr dst_mod = smWin.show();
      if(dst_mod != NULL) {

	selected_mod->remove_gate(gate);
	dst_mod->add_gate(gate);

	insert_gates(selected_mod);
	insert_ports(selected_mod);
      }
    }
  }
}


sigc::signal<void, degate::PlacedLogicModelObject_shptr>&
ModuleWin::signal_goto_button_clicked() {
  return signal_goto_button_clicked_;
}


void ModuleWin::update() {
  insert_modules();
}


void ModuleWin::update_logic_model(Gtk::TreeModel::Children const& children,
				   Module_shptr parent_module) {

  for(Gtk::TreeModel::Children::iterator iter = children.begin();
      iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;

    Module_shptr module = row[treemodel_modules->m_columns.m_col_object_ptr];
    Module_shptr orig_parent = row[treemodel_modules->m_columns.m_col_orig_parent_ptr];

    assert(module != NULL);

    if(parent_module != NULL && orig_parent != parent_module) {
      orig_parent->remove_module(module);
      parent_module->add_module(module);
    }

    update_logic_model(row.children(), module);
  }
}
