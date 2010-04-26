/*                                                                              
                                                                                
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

#include "GateSelectWin.h"

#include <assert.h>
#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <libglademm.h>
#include <stdlib.h>

using namespace degate;

GateSelectWin::GateSelectWin(Gtk::Window *parent, LogicModel_shptr lmodel) :
  GladeFileLoader("gate_select.glade", "gate_select_dialog") {

  assert(lmodel != NULL);
  assert(parent != NULL);
  this->lmodel = lmodel;
  this->parent = parent;

  if(get_dialog()) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateSelectWin::on_cancel_button_clicked));
    
    get_widget("ok_button", pOkButton);
    if(pOkButton) {
      pOkButton->set_sensitive(false);
      pOkButton->signal_clicked().connect(sigc::mem_fun(*this, &GateSelectWin::on_ok_button_clicked) );         
    }
    
    refListStore = Gtk::ListStore::create(m_Columns);
    
    get_widget("treeview", pTreeView);
    if(pTreeView) {
      pTreeView->set_model(refListStore);
      //pTreeView->append_column("ID", m_Columns.m_col_id);
      pTreeView->append_column("Short Name", m_Columns.m_col_short_name);
      pTreeView->append_column("#", m_Columns.m_col_refcount);
      pTreeView->append_column("Width", m_Columns.m_col_width);
      pTreeView->append_column("Height", m_Columns.m_col_height);
      pTreeView->append_column("Description", m_Columns.m_col_description);

      Gtk::TreeView::Column * pColumn;

      //pColumn = pTreeView->get_column(0);
      //if(pColumn) pColumn->set_sort_column(m_Columns.m_col_id);

      pColumn = pTreeView->get_column(0);
      if(pColumn) pColumn->set_sort_column(m_Columns.m_col_short_name);
      
      pColumn = pTreeView->get_column(1);
      if(pColumn) pColumn->set_sort_column(m_Columns.m_col_refcount);
      
      pColumn = pTreeView->get_column(2);
      if(pColumn) pColumn->set_sort_column(m_Columns.m_col_width);
      
      pColumn = pTreeView->get_column(3);
      if(pColumn) pColumn->set_sort_column(m_Columns.m_col_height);
      
      pColumn = pTreeView->get_column(4);
      if(pColumn) pColumn->set_sort_column(m_Columns.m_col_description);

      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView->get_selection();
      refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &GateSelectWin::on_selection_changed));

      refListStore->set_sort_column_id(m_Columns.m_col_short_name, Gtk::SORT_ASCENDING);
    }
    
    GateLibrary_shptr gate_lib = lmodel->get_gate_library();
    for(GateLibrary::template_iterator iter = gate_lib->begin();
	iter != gate_lib->end(); ++iter) {

      GateTemplate_shptr tmpl = (*iter).second;
      
      Gtk::TreeModel::Row row = *(refListStore->append()); 
	
      row[m_Columns.m_col_id] = tmpl->get_object_id();
      row[m_Columns.m_col_refcount] = tmpl->get_reference_counter();
      row[m_Columns.m_col_width] = tmpl->get_width();
      row[m_Columns.m_col_height] = tmpl->get_height();

      row[m_Columns.m_col_short_name] = tmpl->get_name();
      row[m_Columns.m_col_description] = tmpl->get_description();

    }

  }
}


void GateSelectWin::on_selection_changed() {
  debug(TM, "sth. selected");
  pOkButton->set_sensitive(true);
}

GateSelectWin::~GateSelectWin() {
}

std::list<degate::GateTemplate_shptr> 
GateSelectWin::get_selection(bool allow_multiple_selection) {

  std::list<degate::GateTemplate_shptr> result;

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();

  if(allow_multiple_selection)
    refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);

  get_dialog()->run();

  if(ok_clicked) {

    GateLibrary_shptr gate_lib = lmodel->get_gate_library();
    assert(gate_lib != NULL);

    std::vector<Gtk::TreeModel::Path> pathlist = refTreeSelection->get_selected_rows();

    for(std::vector<Gtk::TreeModel::Path>::iterator iter = pathlist.begin(); 
	iter != pathlist.end(); ++iter) {

      Gtk::TreeModel::Row row = *(refTreeSelection->get_model()->get_iter(*iter));
      
      GateTemplate_shptr tmpl = gate_lib->get_template(row[m_Columns.m_col_id]);
      assert(tmpl != NULL);

      debug(TM, "selected template: [%s]", tmpl->get_name().c_str());
      
      result.push_back(tmpl);
    }
  }

  return result;
}

void GateSelectWin::on_ok_button_clicked() {
  ok_clicked = true;
  get_dialog()->hide();
}

void GateSelectWin::on_cancel_button_clicked() {
  ok_clicked = false;
  get_dialog()->hide();
}

