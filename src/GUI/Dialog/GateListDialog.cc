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

#include "GateListDialog.h"

#include <QHeaderView>

#include <limits>

namespace degate
{
    GateListDialog::GateListDialog(QWidget* parent, Project_shptr project)
            : QDialog(parent), project(project)
    {
        setWindowTitle(tr("Gate list"));

        // Table
        table.setColumnCount(3);
        table.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        QStringList list;
        list.append(tr("ID"));
        list.append(tr("Gate template name"));
        list.append(tr("Gate name"));
        table.setHorizontalHeaderLabels(list);
        table.setColumnWidth(1, 200);
        table.horizontalHeader()->setStretchLastSection(true);

        // ID Filter
        id_filter_label.setText(tr("ID filter:"));
        id_filter_edit.setValidator(new QRegExpValidator(QRegExp("[0-9]*"), this));

        // Gate template name filter
        gate_template_name_filter_label.setText(tr("Gate template filter:"));

        // Gate name filter
        gate_name_filter_label.setText(tr("Gate filter:"));

        // Goto button
        jump_to_button.setText(tr("Goto selected object"));
        jump_to_button.setFocusPolicy(Qt::NoFocus);
        QObject::connect(&jump_to_button, SIGNAL(clicked()), this, SLOT(goto_button_clicked()));

        // Control
        control_layout.addWidget(&id_filter_label, 1, 2);
        control_layout.addWidget(&id_filter_edit, 1, 3);
        control_layout.addWidget(&gate_template_name_filter_label, 1, 4);
        control_layout.addWidget(&gate_template_name_filter_edit, 1, 5);
        control_layout.addWidget(&gate_name_filter_label, 1, 6);
        control_layout.addWidget(&gate_name_filter_edit, 1, 7);
        control_layout.addWidget(&jump_to_button, 2, 7);

        // Control layout stretch
        control_layout.setRowStretch(0, 1);
        control_layout.setRowStretch(1, 1);
        control_layout.setRowStretch(2, 1);

        // Layout
        layout.addWidget(&table, 0, 0);
        layout.addLayout(&control_layout, 1, 0);
        layout.setRowStretch(0, 1);

        setLayout(&layout);

        // Apply filters on return pressed (another solution is to update annotation list for every filters change)
        QObject::connect(&id_filter_edit, SIGNAL(returnPressed()), this, SLOT(update_gate_list()));
        QObject::connect(&gate_template_name_filter_edit, SIGNAL(returnPressed()), this, SLOT(update_gate_list()));
        QObject::connect(&gate_name_filter_edit, SIGNAL(returnPressed()), this, SLOT(update_gate_list()));

        update_gate_list();

        if (parent != nullptr)
            resize(parent->size() * 0.5);
    }

    void GateListDialog::update_gate_list()
    {
        if (project == nullptr)
            return;

        table.clearContents();
        table.setRowCount(0);
        gates.clear();

        for (auto it = project->get_logic_model()->gates_begin(); it != project->get_logic_model()->gates_end(); it++)
        {
            auto gate = it->second;

            if ((!id_filter_edit.text().isEmpty() && gate->get_object_id() != id_filter_edit.text().toUInt()) ||
                gate->get_gate_template()->get_name().find(gate_template_name_filter_edit.text().toStdString()) == std::string::npos ||
                gate->get_name().find(gate_name_filter_edit.text().toStdString()) == std::string::npos)
                continue;

            table.insertRow(table.rowCount());

            // ID
            auto id_item = new QTableWidgetItem(QString::number(gate->get_object_id()));
            id_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            table.setItem(table.rowCount() - 1, 0, id_item);

            // Gate template name
            auto gate_template_name_item = new QTableWidgetItem(QString::fromStdString(gate->get_gate_template()->get_name()));
            gate_template_name_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            table.setItem(table.rowCount() - 1, 1, gate_template_name_item);

            // Gate name
            auto gate_name_item = new QTableWidgetItem(QString::fromStdString(gate->get_name()));
            gate_name_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            table.setItem(table.rowCount() - 1, 2, gate_name_item);

            gates[table.rowCount() - 1] = gate;
        }
    }

    void GateListDialog::goto_button_clicked()
    {
        if (table.selectedItems().empty())
            return;

        PlacedLogicModelObject_shptr object = nullptr;

        object = gates[table.selectedItems().front()->row()];

        if (object != nullptr)
                emit goto_object(object);
    }
}
