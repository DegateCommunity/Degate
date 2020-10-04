/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "GateLibraryDialog.h"

namespace degate
{
    GateLibraryDialog::GateLibraryDialog(QWidget* parent, const Project_shptr& project)
            : QDialog(parent), project(project), list(parent, project, false)
    {
        setWindowTitle(tr("Gate library"));
        resize(300, 400);

        // Buttons text
        edit_gate_button.setText(tr("Edit"));
        remove_gate_button.setText(tr("Remove"));
        validate_button.setText(tr("Ok"));

        // Layout
        buttons_layout.addWidget(&edit_gate_button);
        buttons_layout.addWidget(&remove_gate_button);
        buttons_layout.addWidget(&validate_button);

        layout.addWidget(&list);
        layout.addLayout(&buttons_layout);

        setLayout(&layout);

        QObject::connect(&edit_gate_button, SIGNAL(clicked()), this, SLOT(on_gate_edit_button()));
        QObject::connect(&remove_gate_button, SIGNAL(clicked()), this, SLOT(on_gate_remove_button()));
        QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(close()));
    }

    void GateLibraryDialog::on_gate_edit_button()
    {
        if (!list.has_selection())
            return;

        std::vector<GateTemplate_shptr> selected_gates = list.get_selected_gates();

        for (auto& e : selected_gates)
        {
            GateEditDialog dialog(this, e, project);
            dialog.exec();
        }

        list.update_list();
    }

    void GateLibraryDialog::on_gate_remove_button()
    {
        if (!list.has_selection())
            return;

        std::vector<GateTemplate_shptr> selected_gates = list.get_selected_gates();

        for (auto& e : selected_gates)
        {
            // Ask for confirmation
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this,
                                          tr("Remove gate template"),
                                          tr("Are you sure you want to delete the '%1' gate template and all linked gate instances ?")
                                          .arg(QString::fromStdString(e->get_name())),
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply != QMessageBox::Yes)
                continue;

            // Search every gate instance
            std::vector<Gate_shptr> delete_list;
            for (auto iter = project->get_logic_model()->gates_begin(); iter != project->get_logic_model()->gates_end(); ++iter)
            {
                if (iter->second->get_gate_template() == e)
                    delete_list.push_back(iter->second);
            }

            // Delete all gate instances
            for (auto& gate : delete_list)
                project->get_logic_model()->remove_object(gate);

            // Delete gate template
            project->get_logic_model()->remove_gate_template(e);
        }

        list.update_list();
    }
}
