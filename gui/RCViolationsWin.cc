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

#include "RCViolationsWin.h"

#include <gdkmm/window.h>
#include <gtkmm/stock.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <set>

#include <globals.h>

#include <boost/lexical_cast.hpp>

using namespace degate;


RCViolationsWin::RCViolationsWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel,
				   degate::RCBase::container_type & blacklist) :
  GladeFileLoader("rc_violations_list.glade", "rc_violations_list_dialog"),
  _blacklist(blacklist) {

  this->lmodel = lmodel;
  this->parent = parent;
  assert(lmodel);


  if(get_dialog()) {

    // connect signals
    get_widget("close_button", pCloseButton);
    if(pCloseButton)
      pCloseButton->signal_clicked().connect(sigc::mem_fun(*this, &RCViolationsWin::on_close_button_clicked));

    get_widget("goto_button", pGotoButton);
    if(pGotoButton) {
      pGotoButton->grab_focus();
      pGotoButton->signal_clicked().connect(sigc::mem_fun(*this, &RCViolationsWin::on_goto_button_clicked) );
    }

    get_widget("ignore_button", pIgnoreRCButton);
    if(pIgnoreRCButton) {
      pIgnoreRCButton->signal_clicked().connect(sigc::mem_fun(*this, &RCViolationsWin::on_ignore_button_clicked) );
    }

    get_widget("update_button", pUpdateButton);
    if(pUpdateButton) {
      pUpdateButton->signal_clicked().connect(sigc::mem_fun(*this, &RCViolationsWin::on_update_button_clicked) );
    }

    get_widget("label_num_violations", pNumViolationsLabel);
    assert(pNumViolationsLabel != NULL);

    get_widget("notebook", notebook);
    assert(notebook != NULL);
    if(notebook != NULL) {
      notebook->signal_switch_page().connect(sigc::mem_fun(*this, &RCViolationsWin::on_page_switch) );
    }

    refListStore = Gtk::ListStore::create(m_Columns);
    pTreeView = init_list(refListStore, m_Columns, "treeview", 
			  sigc::mem_fun(*this, &RCViolationsWin::on_selection_changed));

    refListStore_blacklist = Gtk::ListStore::create(m_Columns_blacklist);
    pTreeView_blacklist = init_list(refListStore_blacklist, m_Columns_blacklist, "treeview_blacklist", 
				    sigc::mem_fun(*this, &RCViolationsWin::on_selection_changed));


    disable_widgets();
  }

}

RCViolationsWin::~RCViolationsWin() {
}


Gtk::TreeView* RCViolationsWin::init_list(Glib::RefPtr<Gtk::ListStore> refListStore, 
					   RCViolationsModelColumns & m_Columns, 
					   Glib::ustring const & widget_name,
					   sigc::slot< void > fnk) {


  Gtk::TreeView* pTreeView;
  
  get_widget(widget_name, pTreeView);
  if(pTreeView) {
    pTreeView->set_model(refListStore);

    Gtk::CellRendererText * pRenderer = Gtk::manage( new Gtk::CellRendererText());
    Gtk::TreeView::Column * pColumn;


    /*
     * col 0
     */
    
    pTreeView->append_column("Layer", *pRenderer);
    pColumn = pTreeView->get_column(0);
    pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_layer);
    pColumn->set_resizable(true);
    pColumn->set_sort_column(m_Columns.m_col_layer);
    
    /*
     * col 1
     */
    
    pTreeView->append_column("Class", *pRenderer);
    pColumn = pTreeView->get_column(1);
    pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_violation_class);
    pColumn->set_resizable(true);
    pColumn->set_sort_column(m_Columns.m_col_violation_class);
    
    /*
     * col 2
     */
    
    pTreeView->append_column("Severity", *pRenderer);
    pColumn = pTreeView->get_column(2);
    pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_severity);
    pColumn->set_resizable(true);
    pColumn->set_sort_column(m_Columns.m_col_severity);
    
    /*
     * col 3
     */
    
    pTreeView->append_column("Description", *pRenderer);
    pColumn = pTreeView->get_column(3);
    pColumn->add_attribute(*pRenderer, "text", m_Columns.m_col_violation_description);
    pColumn->set_resizable(true);
    pColumn->set_sort_column(m_Columns.m_col_violation_description);

    // signal
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView->get_selection();
    refTreeSelection->signal_changed().connect(fnk);

    refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
  }
  return pTreeView;
}



void RCViolationsWin::add_to_list(RCViolation_shptr v,
				   Gtk::ListStore::iterator iter, 
				   RCViolationsModelColumns & m_Columns) {

  Gtk::TreeModel::Row row = *iter;

  PlacedLogicModelObject_shptr plo = v->get_object();
  row[m_Columns.m_col_object_ptr] = plo;
  row[m_Columns.m_col_layer] = boost::lexical_cast<Glib::ustring>(plo->get_layer()->get_layer_pos());
  row[m_Columns.m_col_violation_class] = v->get_rc_violation_class();
  row[m_Columns.m_col_violation_description] = v->get_problem_description();
  row[m_Columns.m_col_severity] = v->get_severity_as_string();
  row[m_Columns.m_col_rcv] = v;
}


void RCViolationsWin::run_checks() {
  
  rc.run(lmodel);
  RCVContainer const& violations = rc.get_rc_violations();
  
  clear_list();

  update_first_page();

  RCVContainer remove_from_blacklist;

  BOOST_FOREACH(RCViolation_shptr v, _blacklist) {
    // check
    if(violations.contains(v))
      add_to_list(v, refListStore_blacklist->append(), m_Columns_blacklist);
    else 
      remove_from_blacklist.push_back(v);
  }

  BOOST_FOREACH(RCViolation_shptr v, remove_from_blacklist) {
    _blacklist.erase(v);
  }

  update_stats();
}

void RCViolationsWin::show() {
  get_dialog()->show();
}

void RCViolationsWin::on_selection_changed() {

  bool first_page = notebook->get_current_page() == 0;

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
    first_page ? pTreeView->get_selection() :  pTreeView_blacklist->get_selection();

  if(refTreeSelection) {
    std::vector<Gtk::TreeModel::Path> pathlist = refTreeSelection->get_selected_rows();
    
    pGotoButton->set_sensitive(pathlist.size() == 1);
    pIgnoreRCButton->set_sensitive(pathlist.size() > 0);
  }
}


void RCViolationsWin::update_first_page() {

  refListStore->clear();

  RCVContainer const& violations = rc.get_rc_violations();

  BOOST_FOREACH(RCViolation_shptr v, violations) {
    // check if violation is blacklisted
    if(!_blacklist.contains(v))
       add_to_list(v, refListStore->append(), m_Columns);
  }
}


void RCViolationsWin::update_stats() {
  size_t stat = 0;

  if(notebook->get_current_page() == 0)
    stat = refListStore->children().size();
  else
    stat = refListStore_blacklist->children().size();

  boost::format fmt("%1%");
  fmt % stat;
  pNumViolationsLabel->set_text(fmt.str());
}

void RCViolationsWin::on_page_switch(GtkNotebookPage* w, guint page) {
  if(page == 0) {
    pIgnoreRCButton->set_label("Discard");
  }
  else {
    pIgnoreRCButton->set_label("Reset");
  }

  update_stats();
}

void RCViolationsWin::clear_list() {
  refListStore->clear();
  refListStore_blacklist->clear();
  pGotoButton->set_sensitive(false);
  pIgnoreRCButton->set_sensitive(false);
  pNumViolationsLabel->set_text("0");
}

void RCViolationsWin::disable_widgets() {
  pGotoButton->set_sensitive(false);
  pIgnoreRCButton->set_sensitive(false);
  clear_list();

}

void RCViolationsWin::on_close_button_clicked() {
  get_dialog()->hide();
}


void RCViolationsWin::on_goto_button_clicked() {

  bool first_page = notebook->get_current_page() == 0;

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
    first_page ? pTreeView->get_selection() :  pTreeView_blacklist->get_selection();


  if(refTreeSelection) {

    std::vector<Gtk::TreeModel::Path> pathlist = refTreeSelection->get_selected_rows();

    for(std::vector<Gtk::TreeModel::Path>::iterator iter = pathlist.begin();
	iter != pathlist.end(); ++iter) {

      Gtk::TreeModel::Row row = *(refTreeSelection->get_model()->get_iter(*iter));
      
      PlacedLogicModelObject_shptr object_ptr =
	row[first_page ? m_Columns.m_col_object_ptr :  m_Columns_blacklist.m_col_object_ptr];
            
      assert(object_ptr != NULL);
      
      signal_goto_button_clicked_(object_ptr);
    }
  }
}

void RCViolationsWin::on_update_button_clicked() {
  run_checks();
}

void RCViolationsWin::on_ignore_button_clicked() {
  bool first_page = notebook->get_current_page() == 0;
  bool refresh_needed = false;

  std::list<Gtk::TreeIter> remove_things;

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
    first_page ? pTreeView->get_selection() :  pTreeView_blacklist->get_selection();

  if(refTreeSelection) {

    std::vector<Gtk::TreeModel::Path> pathlist = refTreeSelection->get_selected_rows();

    for(std::vector<Gtk::TreeModel::Path>::iterator iter = pathlist.begin();
	iter != pathlist.end(); ++iter) {

      Gtk::TreeModel::Row row = *(refTreeSelection->get_model()->get_iter(*iter));
    
      RCViolation_shptr rcv =
	row[first_page ? m_Columns.m_col_rcv : m_Columns_blacklist.m_col_rcv];

      if(first_page) {
	remove_things.push_back(refTreeSelection->get_model()->get_iter (*iter));

	if(!_blacklist.contains(rcv)) {
	  if(rcv != NULL) _blacklist.push_back(rcv);
	  add_to_list(rcv, refListStore_blacklist->append(), 
		      m_Columns_blacklist);
	}
      }
      else {
	remove_things.push_back(refTreeSelection->get_model()->get_iter (*iter));
	_blacklist.erase(rcv);
	refresh_needed = true;
      }

    }
    
    BOOST_FOREACH(Gtk::TreeIter & i, remove_things) {
      if(first_page)
	refListStore->erase(i);
      else
	refListStore_blacklist->erase(i);
    }

    if(refresh_needed) {
      Gtk::MessageDialog dialog(*this, "Should degate re-run the RC checks?", 
				true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("RRC Checks");
      if(dialog.run() == Gtk::RESPONSE_YES) run_checks();      
    }

    update_first_page();
    update_stats();
  }
}

sigc::signal<void, degate::PlacedLogicModelObject_shptr>&
RCViolationsWin::signal_goto_button_clicked() {
  return signal_goto_button_clicked_;
}


void RCViolationsWin::objects_removed() {
  disable_widgets();
}

void RCViolationsWin::object_removed(degate::PlacedLogicModelObject_shptr obj_ptr) {
  disable_widgets();
}
