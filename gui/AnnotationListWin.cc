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

#include "AnnotationListWin.h"

#include <gdkmm/window.h>
#include <gtkmm/stock.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <set>
#include <boost/lexical_cast.hpp>

#include <globals.h>
#include <boost/lexical_cast.hpp>

using namespace degate;

AnnotationListWin::AnnotationListWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel) :
  GladeFileLoader("annotation_list.glade", "annotation_list_dialog") {

  this->lmodel = lmodel;
  this->parent = parent;
  assert(lmodel);

  if(pDialog) {

    // connect signals
    refXml->get_widget("close_button", pCloseButton);
    if(pCloseButton)
      pCloseButton->signal_clicked().connect(sigc::mem_fun(*this, &AnnotationListWin::on_close_button_clicked));
    
    refXml->get_widget("goto_button", pGotoButton);
    if(pGotoButton) {
      pGotoButton->grab_focus();
      pGotoButton->signal_clicked().connect(sigc::mem_fun(*this, &AnnotationListWin::on_goto_button_clicked) );
    }

    refXml->get_widget("entry_filter_by_layer", entry_filter_by_layer);
    if(entry_filter_by_layer)
      entry_filter_by_layer->signal_changed().connect(sigc::mem_fun(*this, &AnnotationListWin::on_entry_changed) );

    refXml->get_widget("entry_filter_by_class", entry_filter_by_class);
    if(entry_filter_by_class)
      entry_filter_by_class->signal_changed().connect(sigc::mem_fun(*this, &AnnotationListWin::on_entry_changed) );

    
   
    refListStore = Gtk::ListStore::create(m_Columns);
    
    refXml->get_widget("treeview", pTreeView);
    if(pTreeView) {
      pTreeView->set_model(refListStore);
      
      pTreeView->append_column("Layer", m_Columns.m_col_layer_pos);
      pTreeView->append_column("Class", m_Columns.m_col_annotation_class);

      {
	int view_column = pTreeView->append_column_editable("Name", m_Columns.m_col_name);
	Gtk::CellRenderer *renderer = pTreeView->get_column_cell_renderer(view_column - 1);
	Gtk::CellRendererText *text_renderer = dynamic_cast<Gtk::CellRendererText *>(renderer);
	if(text_renderer) {
	  text_renderer->signal_edited().connect(sigc::mem_fun(*this, &AnnotationListWin::on_edited_name));
	  text_renderer->property_editable() = true;
	}
      }

      {
	int view_column = pTreeView->append_column_editable("Description", m_Columns.m_col_description);
	
	Gtk::CellRenderer *renderer = pTreeView->get_column_cell_renderer(view_column - 1);
	Gtk::CellRendererText *text_renderer = dynamic_cast<Gtk::CellRendererText *>(renderer);
	if(text_renderer) {
	  text_renderer->signal_edited().connect(sigc::mem_fun(*this, &AnnotationListWin::on_edited_description));
	  text_renderer->property_editable() = true;
	}
      }

      // signal
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView->get_selection();
      refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &AnnotationListWin::on_selection_changed));

    }
    
  }

}



AnnotationListWin::~AnnotationListWin() {
}

void AnnotationListWin::on_edited_name(const Glib::ustring & path, const Glib::ustring & new_text) {
  Gtk::TreePath tpath(path);
  Gtk::TreeModel::iterator iter = refListStore->get_iter(tpath);
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    row[m_Columns.m_col_name] = new_text;
    Annotation_shptr a = row[m_Columns.m_col_object_ptr];
    assert(a != NULL);
    a->set_name(new_text.c_str());
  }

}

void AnnotationListWin::on_edited_description(const Glib::ustring & path, const Glib::ustring & new_text) {
  Gtk::TreePath tpath(path);
  Gtk::TreeModel::iterator iter = refListStore->get_iter(tpath);
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    row[m_Columns.m_col_description] = new_text;
    Annotation_shptr a = row[m_Columns.m_col_object_ptr];
    assert(a != NULL);
    a->set_description(new_text.c_str());
  }
}

void AnnotationListWin::refresh() {
  clear_list();

  Annotation::class_id_t class_id = 0;
  layer_position_t layer_pos = 0;

  try {
    class_id = boost::lexical_cast<Annotation::class_id_t>(entry_filter_by_class->get_text());
    layer_pos = boost::lexical_cast<layer_position_t>(entry_filter_by_layer->get_text());
  }
  catch(boost::bad_lexical_cast &) {
  }

  // there should be no exception, but else we will pass it 
  
  for(LogicModel::annotation_collection::iterator iter = lmodel->annotations_begin();
      iter != lmodel->annotations_end(); ++iter) {
    
    Annotation_shptr annotation = iter->second;
    
    Layer_shptr layer = annotation->get_layer();
    
    bool add = true;
    
    if(entry_filter_by_layer->get_text().size() > 0 &&
       layer->get_layer_pos() != layer_pos) add = false;
    
    if(entry_filter_by_class->get_text().size() > 0 &&
       annotation->get_class_id() != class_id) add = false;
    
    if(add) {
      Gtk::TreeModel::Row row = *(refListStore->append());
      
      row[m_Columns.m_col_layer_pos] = layer->get_layer_pos();
      row[m_Columns.m_col_annotation_class] = annotation->get_class_id();
      row[m_Columns.m_col_name] = annotation->get_name();
      row[m_Columns.m_col_description] = annotation->get_description();
      row[m_Columns.m_col_object_ptr] = annotation;
    }
  }
  
}

void AnnotationListWin::show() {

  refresh();
  pDialog->show();
}

void AnnotationListWin::clear_list() {
  refListStore->clear();
  pGotoButton->set_sensitive(false);
}


void AnnotationListWin::on_close_button_clicked() {
  pDialog->hide();
}

void AnnotationListWin::on_goto_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Annotation_shptr object_ptr(row[m_Columns.m_col_object_ptr]);
      assert(object_ptr != NULL);

      signal_goto_button_clicked_(object_ptr);
    }
  }
}

sigc::signal<void, degate::PlacedLogicModelObject_shptr>& 
AnnotationListWin::signal_goto_button_clicked() {
  return signal_goto_button_clicked_;
}


void AnnotationListWin::on_selection_changed() {
  pGotoButton->set_sensitive(true);
}

void AnnotationListWin::on_entry_changed() {
  refresh();
}
