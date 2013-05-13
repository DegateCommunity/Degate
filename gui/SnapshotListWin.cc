/*

This file is part of the IC reverse engineering tool degate.

Copyright 2012 Robert Nitsch

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

#include "SnapshotListWin.h"
#include "MainWin.h"
#include "GladeFileLoader.h"
#include "Project.h"

#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <gtkmm/stock.h>
#include <gdkmm/window.h>
#include <libglademm.h>

#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace degate;

SnapshotListWin::SnapshotListWin(Gtk::Window *parent)
  : GladeFileLoader("snapshot_list.glade", "snapshot_list_dialog")
{
  this->parent = parent;
  this->mainwin = dynamic_cast<MainWin*>(parent);
  assert(mainwin);

  if(get_dialog()) {
    // Get the Glade-instantiated button, and connect a signal handler.
    Gtk::Button *pButton = NULL;

    // Connect signals.
    get_widget("close_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &SnapshotListWin::on_close_button_clicked));
    }
    
    get_widget("add_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &SnapshotListWin::on_add_button_clicked));
    }
    
    get_widget("remove_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &SnapshotListWin::on_remove_button_clicked));
      pButton->set_sensitive(false);
    }
    
    get_widget("clear_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &SnapshotListWin::on_clear_button_clicked));
    }
    
    get_widget("revert_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &SnapshotListWin::on_revert_button_clicked));
      pButton->set_sensitive(false);
    }
    
    // Set up treeview.
    refListStore = Gtk::ListStore::create(m_Columns);
    get_widget("treeview", pTreeView);
    if(pTreeView) {
      pTreeView->get_selection()->signal_changed().connect(sigc::mem_fun(*this, &SnapshotListWin::on_selection_changed));
      
      pTreeView->set_model(refListStore);
      pTreeView->append_column("Datetime", m_Columns.m_col_datetime);
      pTreeView->append_column_editable("Title", m_Columns.m_col_title);

      Gtk::TreeView::Column * pColumn;

      pColumn = pTreeView->get_column(0);
      if(pColumn) {
        pColumn->set_sort_column(m_Columns.m_col_datetime);
      }
      
      pColumn = pTreeView->get_column(1);
      if(pColumn) {
        pColumn->set_sort_column(m_Columns.m_col_title);
      }
      
      refListStore->set_sort_column_id(m_Columns.m_col_timestamp, Gtk::SORT_DESCENDING);
      
      Gtk::CellRendererText * rendererText;
      rendererText = dynamic_cast<Gtk::CellRendererText *>(pTreeView->get_column_cell_renderer(1)); 
      rendererText->signal_edited().connect(sigc::mem_fun(*this, &SnapshotListWin::on_snapshot_title_edited));
    }

    // Fill treeview.
    std::vector<ProjectSnapshot_shptr> snapshots = mainwin->get_snapshots();
    for (auto it = snapshots.begin(); it != snapshots.end(); ++it) {
      fill_row(*(refListStore->append()), *it);
    }
  }
  else {
    std::cout << "Error: can't find gate_list_dialog" << std::endl;
  }
}

ProjectSnapshot_shptr SnapshotListWin::get_selected_snapshot() const {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
  Gtk::TreeModel::Row row = *iter;
  if (iter) {
    return row[m_Columns.m_col_ptr];
  } else {
    return ProjectSnapshot_shptr();
  }
}

void SnapshotListWin::fill_row(Gtk::TreeModel::Row const& row, const ProjectSnapshot_shptr &ss) {
  boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
  row[m_Columns.m_col_timestamp] = long((ss->datetime - epoch).total_milliseconds());
  
  // Format datetime.
  std::stringstream buff;
  boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%d-%b, %H:%M:%S");
  buff.imbue(std::locale(buff.getloc(), facet));
  buff << ss->datetime;
  row[m_Columns.m_col_datetime] = buff.str();
  
  row[m_Columns.m_col_title] = ss->title;
  row[m_Columns.m_col_ptr] = ss;
}

SnapshotListWin::~SnapshotListWin() {
}

void SnapshotListWin::run() {
  get_dialog()->run();
}

void SnapshotListWin::on_close_button_clicked() {
  get_dialog()->hide();
}

void SnapshotListWin::on_add_button_clicked() {
  ProjectSnapshot_shptr ss = mainwin->create_snapshot("snapshot");
  fill_row(*(refListStore->append()), ss);
}

void SnapshotListWin::on_remove_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if (refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if (iter) {
      Gtk::TreeModel::Row row = *iter;

      Gtk::MessageDialog dialog(*parent, "Are you sure you want to remove selected snapshot(s)?",
				true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("Warning");
      if (dialog.run() == Gtk::RESPONSE_YES) {
        dialog.hide();

        ProjectSnapshot_shptr ss = row[m_Columns.m_col_ptr];
        mainwin->remove_snapshot(ss);
        refListStore->erase(iter);
        
        get_dialog()->present();
      }
    }
  }
}

void SnapshotListWin::on_clear_button_clicked() {
  Gtk::MessageDialog dialog(*parent, "Are you sure you want to remove all snapshots?",
          true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
  dialog.set_title("Warning");
  if (dialog.run() == Gtk::RESPONSE_YES) {
    dialog.hide();
    refListStore->clear();
    mainwin->clear_snapshots();
  }
  get_dialog()->present();
}

void SnapshotListWin::on_revert_button_clicked() {
  ProjectSnapshot_shptr ss = get_selected_snapshot();
  if (ss.get() != nullptr) {
    mainwin->revert_to_snapshot(ss);
    get_dialog()->hide();
  }
}

void SnapshotListWin::on_selection_changed() {
  Gtk::Button *pButton = NULL;
  bool sensitive = (get_selected_snapshot().get() != nullptr);
  get_widget("revert_button", pButton);
  if (pButton) {
    pButton->set_sensitive(sensitive);
  }
  get_widget("remove_button", pButton);
  if (pButton) {
    pButton->set_sensitive(sensitive);
  }
}

void SnapshotListWin::on_snapshot_title_edited(const Glib::ustring& path, const Glib::ustring& new_text) {
  auto ss = get_selected_snapshot();
  if (ss.get() != nullptr) {
    ss->title = new_text;
  }
}
