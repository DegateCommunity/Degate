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

#include <DesignRuleChecker.h>
#include "DRCViolationsWin.h"

#include <gdkmm/window.h>
#include <gtkmm/stock.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <set>

#include <globals.h>

#define MY_WHITE "#ffffff"
#define MY_BLUE  "#d0d0ff"

using namespace degate;


DRCViolationsWin::DRCViolationsWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel) :
  GladeFileLoader("drc_violations_list.glade", "drc_violations_list_dialog") {

  this->lmodel = lmodel;
  this->parent = parent;
  assert(lmodel);

  if(get_dialog()) {
   
    // connect signals
    get_widget("close_button", pCloseButton);
    if(pCloseButton)
      pCloseButton->signal_clicked().connect(sigc::mem_fun(*this, &DRCViolationsWin::on_close_button_clicked));
    
    get_widget("goto_button", pGotoButton);
    if(pGotoButton) {
      pGotoButton->grab_focus();
      pGotoButton->signal_clicked().connect(sigc::mem_fun(*this, &DRCViolationsWin::on_goto_button_clicked) );
    }
    
    get_widget("ignore_button", pIgnoreDRCButton);
    if(pIgnoreDRCButton) {
      pIgnoreDRCButton->signal_clicked().connect(sigc::mem_fun(*this, &DRCViolationsWin::on_ignore_button_clicked) );
    }

    get_widget("update_button", pUpdateButton);
    if(pUpdateButton) {
      pUpdateButton->signal_clicked().connect(sigc::mem_fun(*this, &DRCViolationsWin::on_update_button_clicked) );
    }
    
    refListStore = Gtk::ListStore::create(m_Columns);
    
    get_widget("treeview", pTreeView);
    if(pTreeView) {
      pTreeView->set_model(refListStore);
      
      Gtk::CellRendererText * pRenderer = Gtk::manage( new Gtk::CellRendererText()); 
      Gtk::TreeView::Column * pColumn;
      
      
      /*
       * col 0
       */
      
      pTreeView->append_column("Class", *pRenderer);
      pColumn = pTreeView->get_column(0);
      pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_violation_class);
      pColumn->add_attribute(*pRenderer, "background", m_Columns.color_);
      pColumn->set_resizable(true);
      pColumn->set_sort_column(m_Columns.m_col_violation_class);
      
      /*
       * col 1
       */
      
      pTreeView->append_column("Description", *pRenderer);
      pColumn = pTreeView->get_column(1);
      pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_violation_description);
      pColumn->add_attribute(*pRenderer, "background", m_Columns.color_);
      pColumn->set_resizable(true);
      pColumn->set_sort_column(m_Columns.m_col_violation_description);
     

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView->get_selection();
      refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &DRCViolationsWin::on_selection_changed));      
    }    

    disable_widgets();
  }

  //get_dialog()->set_transient_for(*parent);
}

DRCViolationsWin::~DRCViolationsWin() {
}


void DRCViolationsWin::run_checks() {
  DesignRuleChecker drc;
  drc.run(lmodel);

  clear_list();
  Gtk::TreeModel::Row row;

  Glib::ustring current_color = MY_WHITE;

  BOOST_FOREACH(DRCBase::container_type::value_type v, drc.get_drc_violations()) {
    row = *(refListStore->append()); 

    row[m_Columns.m_col_object_ptr] = v->get_object();
    row[m_Columns.m_col_violation_class] = v->get_drc_violation_class();
    row[m_Columns.m_col_violation_description] = v->get_problem_description();

    row[m_Columns.color_] = current_color;

    if(current_color == MY_BLUE) current_color = MY_WHITE;
    else current_color = MY_BLUE;
  }
}

void DRCViolationsWin::show() {
  get_dialog()->show();
}

void DRCViolationsWin::on_selection_changed() {
  pGotoButton->set_sensitive(true);
  pIgnoreDRCButton->set_sensitive(true);
}


void DRCViolationsWin::clear_list() {
  refListStore->clear();
  pGotoButton->set_sensitive(false);
  pIgnoreDRCButton->set_sensitive(false);
}

void DRCViolationsWin::disable_widgets() {
  pGotoButton->set_sensitive(false);
  pIgnoreDRCButton->set_sensitive(false);
  clear_list();

}

void DRCViolationsWin::on_close_button_clicked() {
  get_dialog()->hide();
}


void DRCViolationsWin::on_goto_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      PlacedLogicModelObject_shptr object_ptr(row[m_Columns.m_col_object_ptr]);

      if(object_ptr == NULL) {
	object_ptr = row[m_Columns.m_col_object_ptr];
      }

      assert(object_ptr != NULL);

      if(GatePort_shptr gate_port = std::tr1::dynamic_pointer_cast<GatePort>(object_ptr))
	object_ptr = gate_port->get_gate();
      
      signal_goto_button_clicked_(object_ptr);
    }
  }
}

void DRCViolationsWin::on_update_button_clicked() {
  run_checks();
}

void DRCViolationsWin::on_ignore_button_clicked() {
  Gtk::MessageDialog dialog(*this, "Not implemented, yet.", true, Gtk::MESSAGE_INFO);
  dialog.set_title("Not implemented");
  dialog.run();
}

sigc::signal<void, degate::PlacedLogicModelObject_shptr>& 
DRCViolationsWin::signal_goto_button_clicked() {
  return signal_goto_button_clicked_;
}


void DRCViolationsWin::objects_removed() {
  disable_widgets();
}

void DRCViolationsWin::object_removed(degate::PlacedLogicModelObject_shptr obj_ptr) {
  disable_widgets();
}
