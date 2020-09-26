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

#include "ConnectionInspector.h"

#include <utility>

namespace degate
{

    ConnectionInspector::ConnectionInspector(QWidget* widget, Project_shptr project)
        : QDialog(widget), project(std::move(project))
    {
        setWindowFlags(Qt::Window);
        setWindowTitle(tr("Connection inspector"));

        // Object identity
        object_name_label.setText(tr("Current object:"));
        object_name_value.setText("--");
        object_type_label.setText(tr("Current object type:"));
        object_type_value.setText("--");

        object_layout.addWidget(&object_name_label, 0, 0);
        object_layout.addWidget(&object_name_value, 0, 1);
        object_layout.addWidget(&object_type_label, 1, 0);
        object_layout.addWidget(&object_type_value, 1, 1);

        layout.addLayout(&object_layout);

        // Object connections
        connections_table.setColumnCount(3);
        connections_table.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        QStringList list;
        list.append(tr("Previous"));
        list.append(tr("Current"));
        list.append(tr("Next"));
        connections_table.setHorizontalHeaderLabels(list);
        connections_table.setEditTriggers(QAbstractItemView::NoEditTriggers);
        QObject::connect(&connections_table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(on_item_double_clicked(QTableWidgetItem*)));
        layout.addWidget(&connections_table);

        // Control
        back_button.setText(tr("Back"));
        control_layout.addWidget(&back_button, 0, 1);
        QObject::connect(&back_button, SIGNAL(clicked()), this, SLOT(back()));

        goto_button.setText(tr("Jump to object"));
        control_layout.addWidget(&goto_button, 0, 2);
        QObject::connect(&goto_button, SIGNAL(clicked()), this, SLOT(jump_to_selected_object()));

        close_button.setText(tr("Close"));
        control_layout.addWidget(&close_button, 1, 2);
        QObject::connect(&close_button, SIGNAL(clicked()), this, SLOT(accept()));

        control_layout.setColumnStretch(0, 1);

        layout.addLayout(&control_layout);

        setLayout(&layout);

        resize(400, 500);
    }

    void ConnectionInspector::set_object(const PlacedLogicModelObject_shptr& object)
    {
        if (object == nullptr)
            return;

        object_name_value.setText(QString::fromStdString(object->get_descriptive_identifier()));
        object_type_value.setText(QString::fromStdString(object->get_object_type_name()));
        clear();

        if (back_list.empty() || back_list.back()->get_object_id() != object->get_object_id())
            back_list.push_back(object);

        if (Gate_shptr g = std::dynamic_pointer_cast<Gate>(object))
        {
            for (auto iter = g->ports_begin(); iter != g->ports_end(); ++iter)
            {
                if (*iter != nullptr)
                {
                    show_connections(*iter);
                }
            }
        }
        else if (ConnectedLogicModelObject_shptr o = std::dynamic_pointer_cast<ConnectedLogicModelObject>(object))
        {
            show_connections(o);
        }
    }

    void ConnectionInspector::disable_inspection()
    {
        object_name_label.setText(tr("Current object:"));
        object_name_value.setText("--");
        object_type_label.setText(tr("Current object type:"));
        object_type_value.setText("--");

        clear();
    }

    void ConnectionInspector::jump_to_selected_object()
    {
        if (connections_table.selectedItems().empty())
            return;

        int row = connections_table.selectedItems().first()->row();
        int column = connections_table.selectedItems().first()->column();

        jump_to_object(row, column);
    }

    void ConnectionInspector::back()
    {
        if (back_list.size() > 1)
        {
            back_list.pop_back(); // Remove current object
            PlacedLogicModelObject_shptr o = back_list.back();
            back_list.pop_back();

            if (o == nullptr)
                return;

            set_object(o);
            emit goto_object(o);
        }
    }

    void ConnectionInspector::on_item_double_clicked(QTableWidgetItem* item)
    {
        int row = item->row();
        int column = item->column();

        jump_to_object(row, column);
    }

    void ConnectionInspector::show_connections(const ConnectedLogicModelObject_shptr& object)
    {
        Net_shptr net = object->get_net();

        if (net == nullptr)
        {
            return;
        }

        for (auto iter = net->begin(); iter != net->end(); ++iter)
        {
            object_id_t oid = *iter;
            auto obj_ptr = std::dynamic_pointer_cast<ConnectedLogicModelObject>(project->get_logic_model()->get_object(oid));

            if (obj_ptr != object)
            {
                connections_table.insertRow(connections_table.rowCount());
                connections[connections_table.rowCount() - 1] = { nullptr, nullptr, nullptr };

                // Current
                auto current_item = new QTableWidgetItem(QString::fromStdString(object->get_descriptive_identifier()));
                current_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                connections_table.setItem(connections_table.rowCount() - 1, 1, current_item);

                connections[connections_table.rowCount() - 1].current = object;

                // Connected with

                if (const GatePort_shptr gate_port = std::dynamic_pointer_cast<GatePort>(obj_ptr))
                {
                    const GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();

                    if (!gate_port->has_template_port() || tmpl_port->has_undefined_port_type())
                    {
                        // Previous
                        auto previous_item = new QTableWidgetItem(QString::fromStdString(gate_port->get_descriptive_identifier()));
                        previous_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                        connections_table.setItem(connections_table.rowCount() - 1, 0, previous_item);

                        // Next
                        auto next_item = new QTableWidgetItem(QString::fromStdString(gate_port->get_descriptive_identifier()));
                        next_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                        connections_table.setItem(connections_table.rowCount() - 1, 2, next_item);

                        connections[connections_table.rowCount() - 1].next = gate_port;
                        connections[connections_table.rowCount() - 1].previous = gate_port;
                    }
                    else if (gate_port->has_template_port())
                    {
                        if (tmpl_port->is_inport())
                        {
                             // Next
                            auto next_item = new QTableWidgetItem(QString::fromStdString(gate_port->get_descriptive_identifier()));
                            next_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                            connections_table.setItem(connections_table.rowCount() - 1, 2, next_item);

                            connections[connections_table.rowCount() - 1].next = gate_port;
                        }
                        if (tmpl_port->is_outport())
                        {
                            // Previous
                            auto previous_item = new QTableWidgetItem(QString::fromStdString(gate_port->get_descriptive_identifier()));
                            previous_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                            connections_table.setItem(connections_table.rowCount() - 1, 0, previous_item);

                            connections[connections_table.rowCount() - 1].previous = gate_port;
                        }
                    }
                }
                else
                {
                    // Previous
                    auto previous_item = new QTableWidgetItem(QString::fromStdString(obj_ptr->get_descriptive_identifier()));
                    previous_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    connections_table.setItem(connections_table.rowCount() - 1, 0, previous_item);

                    // Next
                    auto next_item = new QTableWidgetItem(QString::fromStdString(obj_ptr->get_descriptive_identifier()));
                    next_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    connections_table.setItem(connections_table.rowCount() - 1, 2, next_item);

                    connections[connections_table.rowCount() - 1].next = obj_ptr;
                    connections[connections_table.rowCount() - 1].previous = obj_ptr;
                }
            }
        }
    }

    void ConnectionInspector::clear()
    {
        connections_table.clearContents();
        connections_table.setRowCount(0);
        connections.clear();
    }

    void ConnectionInspector::jump_to_object(int row, int column)
    {
        PlacedLogicModelObject_shptr object = nullptr;
        if (column == 0)
        {
            object = static_cast<PlacedLogicModelObject_shptr>(connections[row].previous);
        }
        else if (column == 1)
        {
            object = static_cast<PlacedLogicModelObject_shptr>(connections[row].current);
        }
        else
        {
            object = static_cast<PlacedLogicModelObject_shptr>(connections[row].next);
        }

        if (object == nullptr)
            return;

        set_object(object);
        emit goto_object(object);
    }
}
