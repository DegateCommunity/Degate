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

#include "GateListWin.h"
#include "GateConfigWin.h"
#include "GladeFileLoader.h"

#include "GateTemplate.h"

#include <gdkmm/window.h>
#include <gtkmm/stock.h>
#include <libglademm.h>

#include <assert.h>
#include <iostream>
#include <stdlib.h>

using namespace degate;


GateListWin::GateListWin(Gtk::Window *parent, LogicModel_shptr lmodel,
			 degate::color_t default_frame_col, 
			 degate::color_t default_fill_col) :
  GladeFileLoader("gate_list.glade", "gate_list_dialog"),
  _default_frame_col(default_frame_col),
  _default_fill_col(default_fill_col) {

  this->lmodel = lmodel;
  this->parent = parent;
  assert(lmodel);

  if(get_dialog()) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;

    // connect signals
    get_widget("close_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateListWin::on_close_button_clicked));

    get_widget("add_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateListWin::on_add_button_clicked) );

    get_widget("remove_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateListWin::on_remove_button_clicked) );

    get_widget("edit_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateListWin::on_edit_button_clicked) );

    refListStore = Gtk::ListStore::create(m_Columns);

    get_widget("treeview", pTreeView);
    if(pTreeView) {
      pTreeView->set_model(refListStore);
      //pTreeView->append_column("ID", m_Columns.m_col_id);
      pTreeView->append_column("Short Name", m_Columns.m_col_short_name);
      pTreeView->append_column("#", m_Columns.m_col_refcount);
      pTreeView->append_column("Width", m_Columns.m_col_width);
      pTreeView->append_column("Height", m_Columns.m_col_height);

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


      Gtk::CellRendererText * pRenderer = Gtk::manage( new Gtk::CellRendererText());
      pTreeView->append_column("Fill color", *pRenderer);
      pColumn = pTreeView->get_column(4);
      pColumn->add_attribute(*pRenderer, "background-gdk", m_Columns.color_fill_);
      pColumn->add_attribute(*pRenderer, "xpad", m_Columns.padding_);
      pColumn->add_attribute(*pRenderer, "ypad", m_Columns.padding_);

      pRenderer = Gtk::manage( new Gtk::CellRendererText());
      pTreeView->append_column("Frame color", *pRenderer);
      pColumn = pTreeView->get_column(5);
      pColumn->add_attribute(*pRenderer, "background-gdk", m_Columns.color_frame_);
      pColumn->add_attribute(*pRenderer, "xpad", m_Columns.padding_);
      pColumn->add_attribute(*pRenderer, "ypad", m_Columns.padding_);

      pTreeView->append_column("Description", m_Columns.m_col_description);
      pColumn = pTreeView->get_column(6);
      if(pColumn) pColumn->set_sort_column(m_Columns.m_col_description);

      refListStore->set_sort_column_id(m_Columns.m_col_short_name, Gtk::SORT_ASCENDING);
    }

    GateLibrary_shptr gate_lib = lmodel->get_gate_library();
    for(GateLibrary::template_iterator iter = gate_lib->begin();
	iter != gate_lib->end(); ++iter) {

      fill_row(*(refListStore->append()), (*iter).second);
    }

  }
  else {
    std::cout << "Error: can't find gate_list_dialog" << std::endl;
  }

}

void GateListWin::fill_row(Gtk::TreeModel::Row const& row, std::tr1::shared_ptr<GateTemplate> & tmpl) {
  row[m_Columns.m_col_id] = tmpl->get_object_id();
  row[m_Columns.m_col_refcount] = tmpl->get_reference_counter();

  row[m_Columns.m_col_width] = tmpl->get_width();
  row[m_Columns.m_col_height] = tmpl->get_height();

  row[m_Columns.m_col_short_name] = tmpl->get_name();
  row[m_Columns.m_col_description] = tmpl->get_description();


  row[m_Columns.color_fill_] = get_color(tmpl->get_fill_color(), _default_fill_col);
  row[m_Columns.color_frame_] = get_color(tmpl->get_frame_color(), _default_frame_col);

  row[m_Columns.padding_] = 5;
}

Gdk::Color GateListWin::get_color(color_t col, color_t default_col) {

  Gdk::Color c;
  color_t set_col = col != 0 ? col : default_col;

  c.set_red(MASK_R(set_col) << 8);
  c.set_green(MASK_G(set_col) << 8);
  c.set_blue(MASK_B(set_col) << 8);

  return c;
}

GateListWin::~GateListWin() {
}

void GateListWin::run() {

  get_dialog()->run();
}

void GateListWin::on_close_button_clicked() {
  get_dialog()->hide();
}

void GateListWin::on_add_button_clicked() {

  GateTemplate_shptr tmpl(new GateTemplate());
  assert(tmpl != NULL);

  GateConfigWin gcWin(parent, lmodel, tmpl, _default_frame_col, _default_fill_col);
  if(gcWin.run() == true) {

    std::tr1::shared_ptr<GateTemplate> tmpl_shared_ptr(tmpl);

    lmodel->add_gate_template(tmpl_shared_ptr);

    fill_row(*(refListStore->append()), tmpl_shared_ptr);

  }
}

void GateListWin::on_remove_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(iter) {
      Gtk::TreeModel::Row row = *iter;
      object_id_t obj_id = row[m_Columns.m_col_id];

      Gtk::MessageDialog dialog(*parent, "Are you sure you want to remove selected gate template(s)?",
				true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("Warning");
      if(dialog.run() == Gtk::RESPONSE_YES) {
	dialog.hide();

	GateLibrary_shptr gate_lib = lmodel->get_gate_library();
	GateTemplate_shptr tmpl = gate_lib->get_template(obj_id);

	if(tmpl) {

	  if(tmpl->get_reference_counter() > 0) {
	    Gtk::MessageDialog dialog2(*parent,
				       "The gate template is referenced by placed gates. "
				       "Do you want to remove the gates as well?",
				       true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);

	    if(dialog2.run() == Gtk::RESPONSE_NO) {
	      dialog2.hide();
	      lmodel->remove_template_references(tmpl); // let gates alive
	    }

	  }

	  lmodel->remove_gate_template(tmpl);
	  refListStore->erase(iter);
	}

      }
    }

  }
}

void GateListWin::on_edit_button_clicked() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(iter) {
      Gtk::TreeModel::Row row = *iter;
      object_id_t obj_id = row[m_Columns.m_col_id];

      GateLibrary_shptr gate_lib = lmodel->get_gate_library();
      assert(gate_lib != NULL);
      GateTemplate_shptr tmpl = gate_lib->get_template(obj_id);

      GateConfigWin gcWin(parent, lmodel, tmpl, _default_frame_col, _default_fill_col);
      if(gcWin.run() == true) {
	row[m_Columns.m_col_id] = tmpl->get_object_id();
	row[m_Columns.m_col_refcount] = tmpl->get_reference_counter();

	row[m_Columns.m_col_width] = tmpl->get_width();
	row[m_Columns.m_col_height] = tmpl->get_height();

	row[m_Columns.m_col_short_name] = tmpl->get_name();
	row[m_Columns.m_col_description] = tmpl->get_description();

	row[m_Columns.color_fill_] = get_color(tmpl->get_fill_color(), _default_fill_col);
	row[m_Columns.color_frame_] = get_color(tmpl->get_frame_color(), _default_frame_col);
      }
    }
  }
}

