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

#ifndef __CONNECTIONINSPECTOR_H__
#define __CONNECTIONINSPECTOR_H__

#include "Core/Project/Project.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

#include <vector>

namespace degate
{
    /**
     * Store information about objects (previous, current and next objects) for a row.
     *
     * @see ConnectionInspector
     */
    struct ConnectionRow
    {
        ConnectedLogicModelObject_shptr previous;
        ConnectedLogicModelObject_shptr current;
        ConnectedLogicModelObject_shptr next;
    };

    /**
     * @class ConnectionInspector
     * @brief Display connection information about an object.
     */
    class ConnectionInspector : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * Create the connection inspector window.
         *
         * @param widget : the parent widget.
         * @param project : the current project.
         */
        ConnectionInspector(QWidget* widget, Project_shptr project);
        ~ConnectionInspector() override = default;

        /**
         * Set an object that should be inspected.
         * @param object : the object to inspect.
         */
        void set_object(const degate::PlacedLogicModelObject_shptr& object);

        /**
         * Disable inspection, clear all elements.
         */
        void disable_inspection();

    protected slots:
        /**
         * Jump to selected object. Will call jump_to_object().
         */
        void jump_to_selected_object();

        /**
         * Goto previous inspected object.
         */
        void back();

        /**
         * Called when an item is double clicked.
         *
         * @param item : the double clicked item.
         *
         * @see QTreeWidget
         */
        void on_item_double_clicked(QTableWidgetItem* item);

    protected:
        /**
         * Show connections of a specific object.
         */
        void show_connections(const ConnectedLogicModelObject_shptr& object);

        /**
         * Clear table contents.
         */
        void clear();

        /**
         * Jump to object. Will emit goto_object().
         *
         * @param row : the object row.
         * @param column : the object column.
         */
        void jump_to_object(int row, int column);

    signals:
        /**
         * Sent when the "goto" button is clicked.
         *
         * @param object : the concerned object to go to.
         */
        void goto_object(PlacedLogicModelObject_shptr& object);

    private:
        Project_shptr project = nullptr;
        QVBoxLayout layout;
        std::vector<PlacedLogicModelObject_shptr> back_list;

        // Object identity
        QGridLayout object_layout;
        QLabel object_name_label;
        QLabel object_name_value;
        QLabel object_type_label;
        QLabel object_type_value;

        // Object connections
        QTableWidget connections_table;
        std::map<int, ConnectionRow> connections;

        // Control
        QGridLayout control_layout;
        QPushButton back_button;
        QPushButton goto_button;
        QPushButton close_button;

    };
}

#endif //__CONNECTIONINSPECTOR_H__
