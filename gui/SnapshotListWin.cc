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
#include "GladeFileLoader.h"
#include "Project.h"

#include <gtkmm/stock.h>
#include <gdkmm/window.h>
#include <libglademm.h>

#include <assert.h>
#include <iostream>
#include <stdlib.h>

using namespace degate;

SnapshotListWin::SnapshotListWin(Gtk::Window *parent, Project_shptr project)
  : GladeFileLoader("snapshot_list.glade", "snapshot_list_dialog")
{
  this->project = project;
  this->parent = parent;
  assert(project.get() != nullptr);

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
      pTreeView->append_column("ID", m_Columns.m_col_id);
      pTreeView->append_column("Title", m_Columns.m_col_title);

      Gtk::TreeView::Column * pColumn;

      pColumn = pTreeView->get_column(0);
      if(pColumn) {
        pColumn->set_sort_column(m_Columns.m_col_id);
      }
      
      pColumn = pTreeView->get_column(1);
      if(pColumn) {
        pColumn->set_sort_column(m_Columns.m_col_title);
      }
      
      refListStore->set_sort_column_id(m_Columns.m_col_title, Gtk::SORT_ASCENDING);
    }

    // Fill treeview.
    std::vector<Project::Snapshot> snapshots = project->get_snapshots();
    for (auto it = snapshots.begin(); it != snapshots.end(); ++it) {
      fill_row(*(refListStore->append()), *it);
    }
  }
  else {
    std::cout << "Error: can't find gate_list_dialog" << std::endl;
  }
}

int SnapshotListWin::treeview_get_selected_id() const {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
  Gtk::TreeModel::Row row = *iter;
  if (iter) {
    return row[m_Columns.m_col_id];
  } else {
    return -1;
  }
}

void SnapshotListWin::fill_row(Gtk::TreeModel::Row const& row, const Project::Snapshot &ss) {
  row[m_Columns.m_col_id] = ss.id;
  row[m_Columns.m_col_title] = ss.title;
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
  Project::Snapshot ss = project->create_snapshot("snapshot");
  fill_row(*(refListStore->append()), ss);
}

void SnapshotListWin::on_remove_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if (refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if (iter) {
      Gtk::TreeModel::Row row = *iter;
      int ss_id = row[m_Columns.m_col_id];

      Gtk::MessageDialog dialog(*parent, "Are you sure you want to remove selected snapshot(s)?",
				true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("Warning");
      if (dialog.run() == Gtk::RESPONSE_YES) {
        dialog.hide();

        project->remove_snapshot(ss_id);
        refListStore->erase(iter);
        
        get_dialog()->present();
      }
    }
  }
}

void SnapshotListWin::on_clear_button_clicked() {
  refListStore->clear();
  project->clear_snapshots();
}

void SnapshotListWin::on_revert_button_clicked() {
  int ss_id = treeview_get_selected_id();
  if (ss_id != -1) {
    project->revert_to(ss_id);
    get_dialog()->hide();
  }
}

void SnapshotListWin::on_selection_changed() {
  Gtk::Button *pButton = NULL;
  get_widget("revert_button", pButton);
  if (pButton) {
    pButton->set_sensitive(treeview_get_selected_id() != -1);
  }
}
