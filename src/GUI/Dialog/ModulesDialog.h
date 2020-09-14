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

#ifndef __MODULESDIALOG_H__
#define __MODULESDIALOG_H__

#include "GUI/Dialog/ModuleSelectionDialog.h"

namespace degate
{
    /**
     * @class ModulesDialog
     * @brief Run a dialog that will display all modules and allow interaction (add, remove, see gates in each module...).
     *
     * @see QDialog
     */
    class ModulesDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Create a modules dialog.
         *
         * @param project : the current project.
         * @param parent : the parent widget.
         */
        ModulesDialog(QWidget* parent, Project_shptr project);
        ~ModulesDialog() override = default;

        /**
         * Reload all trees (modules, gates and ports).
         */
        void reload();

    protected:
        /**
         * Insert all modules of the project in the modules tree.
         *
         * @see QTreeWidget
         */
        void inset_modules();

        /**
         * Insert a module and his children (recursive) in the modules tree.
         *
         * @param parent_item : the parent item.
         * @param module : the module to insert.
         * @param parent : the parent module.
         *
         * @see QTreeWidget
         */
        void insert_module(QTreeWidgetItem* parent_item, const Module_shptr& module, const Module_shptr& parent);

        /**
         * Insert all gates of a module in the gates tree.
         *
         * @param module : the module containing gates to insert.
         *
         * @see QTreeWidget
         */
        void insert_gates(const Module_shptr& module);

        /**
         * Insert all module ports of a module in the ports tree.
         *
         * @param module : the module containing module ports to insert.
         *
         * @see QTreeWidget
         */
        void insert_ports(const Module_shptr& module);

    protected slots:
        /**
         * Called when the selected module changed.
         * Can have no selection.
         */
        void on_module_selection_changed();

        /**
         * Called when the selected gate changed.
         * Can have no selection.
         */
        void on_gate_selection_changed();

        /**
         * Called when the selected module port changed.
         * Can have no selection.
         */
        void on_port_selection_changed();

        /**
         * Add a new module, it will be a child of the currently selected module.
         */
        void add_module();

        /**
         * Remove the selected module.
         */
        void remove_module();

        /**
         * Determine module ports for the selected module.
         */
        void determine_module_ports();

        /**
         * Export the selected module.
         * It will open a file explorer to select a folder to export the module in.
         */
        void export_module();

        /**
         * Move the selected gate into a selectable module.
         * It will open a module selection dialog.
         *
         * @see ModuleSelectionDialog
         */
        void move_gate_into_module();

        /**
         * Jump to the selected gate.
         */
        void jump_to_gate();

        /**
         * Jump to the selected port.
         */
        void jump_to_port();

        /**
         * Called when the selected module name or type changed.
         *
         * @param item : the modified module item.
         * @param column : the modified column.
         *
         * @see QTreeWidget
         */
        void on_module_changed(QTreeWidgetItem* item, int column);

        /**
         * Called when a port is double clicked to edit.
         * Useful to allow port edit only for the first column (port name).
         *
         * @param item : the modified port item.
         * @param column : the modified column.
         *
         * @see QTreeWidget
         */
        void on_port_double_clicked(QTreeWidgetItem* item, int column);

        /**
         * Called when the selected port name or type changed.
         *
         * @param item : the modified port item.
         * @param column : the modified column.
         *
         * @see QTreeWidget
         */
        void on_port_changed(QTreeWidgetItem* item, int column);

    signals:
        /**
         * Sent when the "goto" button is clicked.
         *
         * @param object : the concerned object to go to.
         */
        void goto_object(PlacedLogicModelObject_shptr& object);

    private:
        Project_shptr project = nullptr;
        QGridLayout layout;

        // Modules part
        QTreeWidget modules_tree;
        std::map<QTreeWidgetItem*, ModuleItemCollection> modules_map;

        // Gates part
        QTreeWidget gates_tree;
        std::map<QTreeWidgetItem*, Gate_shptr> gates_map;

        // Ports part
        QTreeWidget ports_tree;
        std::map<QTreeWidgetItem*, GatePort_shptr> ports_map;

        // Control part
        QPushButton add_module_button;
        QPushButton remove_module_button;
        QPushButton determine_ports_module_button;
        QPushButton export_module_button;

        QPushButton move_gate_into_module_button;
        QPushButton jump_to_gate_button;

        QPushButton jump_to_port_button;

        QPushButton close_button;
    };
}

#endif //__MODULESDIALOG_H__
