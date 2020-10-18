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

#include "AnnotationListDialog.h"

#include <QHeaderView>

namespace degate
{
    AnnotationListDialog::AnnotationListDialog(QWidget* parent, Project_shptr project)
            : QDialog(parent), project(project)
    {
        setWindowTitle(tr("Annotation list"));

        // Table
        table.setColumnCount(2);
        table.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        QStringList list;
        list.append(tr("Layer"));
        list.append(tr("Content"));
        table.setHorizontalHeaderLabels(list);
        table.horizontalHeader()->setStretchLastSection(true);

        // Filter
        filter_label.setText(tr("Filter:"));
        filter_reset_button.setText(tr("Reset"));
        filter_reset_button.setFocusPolicy(Qt::NoFocus);

        // Goto button
        jump_to_button.setText(tr("Goto selected object"));
        jump_to_button.setFocusPolicy(Qt::NoFocus);
        QObject::connect(&jump_to_button, SIGNAL(clicked()), this, SLOT(goto_button_clicked()));

        // Control
        control_layout.addWidget(&filter_label, 1, 2);
        control_layout.addWidget(&filter_edit, 1, 3);
        control_layout.addWidget(&filter_reset_button, 1, 4);
        control_layout.addWidget(&jump_to_button, 2, 4);

        // Control layout stretch
        control_layout.setRowStretch(0, 1);
        control_layout.setRowStretch(1, 1);
        control_layout.setRowStretch(2, 1);

        // Layout
        layout.addWidget(&table, 0, 0);
        layout.addLayout(&control_layout, 1, 0);
        layout.setRowStretch(0, 1);

        setLayout(&layout);

        // Reset filter
        QObject::connect(&filter_reset_button, SIGNAL(clicked()), this, SLOT(reset_filter()));

        // Apply filter on return pressed (another solution is to update annotation list every filter change)
        QObject::connect(&filter_edit, SIGNAL(returnPressed()), this, SLOT(update_annotation_list()));

        update_annotation_list();

        if (parent != nullptr)
            resize(parent->size() * 0.5);
    }

    void AnnotationListDialog::update_annotation_list()
    {
        if (project == nullptr)
            return;

        table.clearContents();
        table.setRowCount(0);
        annotations.clear();

        for (auto it = project->get_logic_model()->annotations_begin(); it != project->get_logic_model()->annotations_end(); it++)
        {
            auto annotation = it->second;

            if (annotation->get_name().find(filter_edit.text().toStdString()) == std::string::npos)
                continue;

            table.insertRow(table.rowCount());

            // Layer
            auto layer_item = new QTableWidgetItem(QString::number(annotation->get_layer()->get_layer_pos()));
            layer_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            table.setItem(table.rowCount() - 1, 0, layer_item);

            // Content
            auto class_item = new QTableWidgetItem(QString::fromStdString(annotation->get_name()));
            class_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            table.setItem(table.rowCount() - 1, 1, class_item);

            annotations[table.rowCount() - 1] = annotation;
        }
    }

    void AnnotationListDialog::reset_filter()
    {
        filter_edit.setText("");
        update_annotation_list();
    }

    void AnnotationListDialog::goto_button_clicked()
    {
        if (table.selectedItems().empty())
            return;

        PlacedLogicModelObject_shptr object = nullptr;

        object = annotations[table.selectedItems().front()->row()];

        if (object != nullptr)
                emit goto_object(object);
    }
}
