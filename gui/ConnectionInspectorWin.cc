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

#include "ConnectionInspectorWin.h"

#include <gdkmm/window.h>
#include <gtkmm/stock.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <set>

#include <globals.h>

#define MY_GREY "#808080"
#define MY_WHITE "#ffffff"
#define MY_BLUE  "#d0d0ff"
#define DEFAULT_WIDTH 90

using namespace degate;

ConnectionInspectorWin::ConnectionInspectorWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel) :
  GladeFileLoader("connection_inspector.glade", "connection_inspector_dialog") {

  this->lmodel = lmodel;
  this->parent = parent;
  assert(lmodel);

  if(get_dialog()) {
   
    // connect signals
    get_widget("close_button", pCloseButton);
    if(pCloseButton)
      pCloseButton->signal_clicked().connect(sigc::mem_fun(*this, &ConnectionInspectorWin::on_close_button_clicked));
    
    get_widget("goto_button", pGotoButton);
    if(pGotoButton) {
      pGotoButton->grab_focus();
      pGotoButton->signal_clicked().connect(sigc::mem_fun(*this, &ConnectionInspectorWin::on_goto_button_clicked) );
    }
    
    get_widget("back_button", pBackButton);
    if(pBackButton) {
      pBackButton->signal_clicked().connect(sigc::mem_fun(*this, &ConnectionInspectorWin::on_back_button_clicked) );
    }
    
    get_widget("current_object_label", current_object_label);
    get_widget("current_object_type_label", current_object_type_label);
    
    refListStore = Gtk::ListStore::create(m_Columns);
    
    get_widget("treeview", pTreeView);
    if(pTreeView) {
      pTreeView->set_model(refListStore);
      
      Gtk::CellRendererText * pRenderer = Gtk::manage( new Gtk::CellRendererText()); 
      Gtk::TreeView::Column * pColumn;
      
      /*
       * col 0
       */
      pTreeView->append_column("Previous", *pRenderer);
      
      pColumn = pTreeView->get_column(0);
      // text attribute is the text to show
      pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_prev_name);  
      pColumn->add_attribute(*pRenderer, "background", m_Columns.color_); 
      
      pColumn->set_resizable(true); 
      pColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
      pColumn->set_min_width(DEFAULT_WIDTH);
      
      pColumn->set_sort_column(m_Columns.m_col_prev_name);
      //refListStore->set_sort_column(m_Columns.m_col_prev_name, Gtk::SORT_ASCENDING);
      
      /*
       * col 1
       */
      
      pTreeView->append_column("Current", *pRenderer);
      pColumn = pTreeView->get_column(1);
      // text attribute is the text to show
      pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_curr_name);
      pColumn->add_attribute(*pRenderer, "background", m_Columns.color_);
      
      pColumn->set_resizable(true); 
      pColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
      pColumn->set_min_width(DEFAULT_WIDTH);
      
      pColumn->set_sort_column(m_Columns.m_col_curr_name_sort);
      //refListStore->set_sort_column(m_Columns.m_col_curr_name_sort, Gtk::SORT_ASCENDING);
      
      /*
       * col 2
       */
      
      pTreeView->append_column("Next", *pRenderer);
      pColumn = pTreeView->get_column(2);	
      // text attribute is the text to show
      pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_next_name);
      pColumn->add_attribute(*pRenderer, "background", m_Columns.color_);
      
      pColumn->set_resizable(true); 
      pColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
      pColumn->set_min_width(DEFAULT_WIDTH);
      
      pColumn->set_sort_column(m_Columns.m_col_next_name);
      //refListStore->set_sort_column(m_Columns.m_col_next_name, Gtk::SORT_ASCENDING);

      
      refListStore->set_sort_column_id(m_Columns.m_col_curr_name_sort, Gtk::SORT_ASCENDING);
	
      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView->get_selection();
      refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &ConnectionInspectorWin::on_selection_changed));
      
    }
    
  }

  //get_dialog()->set_transient_for(*parent);
  disable_inspection();
}

ConnectionInspectorWin::~ConnectionInspectorWin() {
}

void ConnectionInspectorWin::show() {
  get_dialog()->show();
}


void ConnectionInspectorWin::on_selection_changed() {

  pGotoButton->set_sensitive(true);
}


void ConnectionInspectorWin::set_object(degate::PlacedLogicModelObject_shptr obj_ptr) {

  assert(obj_ptr != NULL);
  if(obj_ptr == NULL) disable_inspection();
  else {

    current_object_label->set_text(obj_ptr->get_descriptive_identifier());
    current_object_type_label->set_text(obj_ptr->get_object_type_name());
    clear_list();

    back_list.push_back(obj_ptr);


    if(Gate_shptr g = std::tr1::dynamic_pointer_cast<Gate>(obj_ptr)) {
      for(Gate::port_iterator iter = g->ports_begin();
	  iter != g->ports_end(); ++iter) {
	Glib::ustring current_color = MY_WHITE;

	if(*iter != NULL)
	  show_connections(*iter, current_color);

	if(current_color == MY_BLUE) current_color = MY_WHITE;
	else current_color = MY_BLUE;
      }
    }
    else if(ConnectedLogicModelObject_shptr o = 
	    std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(obj_ptr)) {
      show_connections(o, MY_WHITE);
    }

  }
}



void ConnectionInspectorWin::show_connections(degate::ConnectedLogicModelObject_shptr src_curr_obj,
					      Glib::ustring current_color) {

  
  assert(src_curr_obj != NULL);
  Gtk::TreeModel::Row row;
  
  Net_shptr net = src_curr_obj->get_net();
  if(net == NULL) {
    //disable_inspection();
    return;
  }


  for(Net::connection_iterator iter = net->begin();
      iter != net->end(); ++iter) {
    
    object_id_t oid = *iter;
    const ConnectedLogicModelObject_shptr obj_ptr = 
      std::tr1::dynamic_pointer_cast<ConnectedLogicModelObject>(lmodel->get_object(oid));

    if(obj_ptr != src_curr_obj) {

      row = *(refListStore->append()); 

      row[m_Columns.color_] = current_color;

      // middle column

      row[m_Columns.m_col_curr_object_ptr] = src_curr_obj;
      row[m_Columns.m_col_curr_name] = src_curr_obj->get_descriptive_identifier();
      row[m_Columns.m_col_curr_name_sort] = src_curr_obj->get_descriptive_identifier();

      if(GatePort_shptr gate_port = std::tr1::dynamic_pointer_cast<GatePort>(src_curr_obj)) {
	  
	if(gate_port->has_template_port()) {
	  
	  GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
	  
	  if(tmpl_port->is_tristate())
	    row[m_Columns.m_col_curr_name_sort] = Glib::ustring("t") + row[m_Columns.m_col_curr_name_sort];
	  else if(tmpl_port->is_outport())
	    row[m_Columns.m_col_curr_name_sort] = Glib::ustring("o") + row[m_Columns.m_col_curr_name_sort];
	  else if(tmpl_port->is_inport())
	    row[m_Columns.m_col_curr_name_sort] = Glib::ustring("i") + row[m_Columns.m_col_curr_name_sort];
	}
      }

      // connected with

      if(const GatePort_shptr gate_port = 
	 std::tr1::dynamic_pointer_cast<GatePort>(obj_ptr)) {

	const GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();

	if(!gate_port->has_template_port() ||
	   tmpl_port->is_tristate() ||
	   tmpl_port->has_undefined_port_type()) {

	  row[m_Columns.m_col_next_name] = gate_port->get_descriptive_identifier();
	  row[m_Columns.m_col_prev_name] = gate_port->get_descriptive_identifier();
	  row[m_Columns.m_col_next_object_ptr] = gate_port;
	  row[m_Columns.m_col_prev_object_ptr] = gate_port;
	}
	else if(gate_port->has_template_port()) {
	  if(tmpl_port->is_inport()) {
	    row[m_Columns.m_col_next_name] = gate_port->get_descriptive_identifier();
	    row[m_Columns.m_col_next_object_ptr] = gate_port;
	  }
	  else {
	    row[m_Columns.m_col_prev_name] = gate_port->get_descriptive_identifier();
	    row[m_Columns.m_col_prev_object_ptr] = gate_port;
	  }
	}
      }
      else {
	row[m_Columns.m_col_next_name] = obj_ptr->get_descriptive_identifier();
	row[m_Columns.m_col_prev_name] = obj_ptr->get_descriptive_identifier();
	
	row[m_Columns.m_col_next_object_ptr] = obj_ptr;
	row[m_Columns.m_col_prev_object_ptr] = obj_ptr;
      }

    }
  }
}


void ConnectionInspectorWin::clear_list() {
  refListStore->clear();
  pGotoButton->set_sensitive(false);
}

void ConnectionInspectorWin::disable_inspection() {
  current_object_label->set_text("---");
  current_object_type_label->set_text("---");
  pGotoButton->set_sensitive(false);
  pBackButton->set_sensitive(false);
  clear_list();
  back_list.clear();

}

void ConnectionInspectorWin::on_close_button_clicked() {
  get_dialog()->hide();
}

void ConnectionInspectorWin::on_back_button_clicked() {
  if(back_list.size() > 1) {

    back_list.pop_back(); // remove current object

    PlacedLogicModelObject_shptr o = back_list.back();
    back_list.pop_back();

    if(back_list.size() == 0) pBackButton->set_sensitive(false);

    set_object(o); // adds current obj to back_list

    if(!signal_goto_button_clicked_.empty()) signal_goto_button_clicked_(o);
  }
}

void ConnectionInspectorWin::on_goto_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      PlacedLogicModelObject_shptr object_ptr(row[m_Columns.m_col_next_object_ptr]);

      if(object_ptr == NULL) {
	object_ptr = row[m_Columns.m_col_prev_object_ptr];
      }

      assert(object_ptr != NULL);

      pBackButton->set_sensitive(true);

      if(GatePort_shptr gate_port = std::tr1::dynamic_pointer_cast<GatePort>(object_ptr))
	object_ptr = gate_port->get_gate();
      
      set_object(object_ptr);

      if(back_list.size() > 0) pBackButton->set_sensitive(true);

      signal_goto_button_clicked_(object_ptr);
    }
  }
}

sigc::signal<void, degate::PlacedLogicModelObject_shptr>& 
ConnectionInspectorWin::signal_goto_button_clicked() {
  return signal_goto_button_clicked_;
}


void ConnectionInspectorWin::objects_removed() {
  disable_inspection();
}

void ConnectionInspectorWin::object_removed(degate::PlacedLogicModelObject_shptr obj_ptr) {
  disable_inspection();
}
