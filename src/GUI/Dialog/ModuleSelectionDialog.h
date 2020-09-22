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

#ifndef __MODULESELECTIONDIALOG_H__
#define __MODULESELECTIONDIALOG_H__

#include "Core/Project/Project.h"

#include <QDialog>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <map>

namespace degate
{
    /**
     * @struct ModuleItemCollection
     * @brief Describe a module as an item for a QTreeWidget.
     *
     * @see QTreeWidget
     * @see ModulesDialog
     */
    struct ModuleItemCollection
    {
        Module_shptr module;
        Module_shptr parent;
    };

    /**
     * @class ModuleSelectionDialog
     * @brief Show a selection dialog to select a module (of the current project).
     */
    class ModuleSelectionDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Create the module selection dialog.
         *
         * @param project : the current project.
         * @param parent : the parent widget.
         */
        ModuleSelectionDialog(Project_shptr project, QWidget* parent);
        ~ModuleSelectionDialog() override = default;

        /**
         * Get the selected module (call it after exec()).
         *
         * @return Returns the selected module. nullptr if no selection/cancel.
         *
         * @see QDialog
         */
        Module_shptr get_selected_module();

    protected:
        /**
         * Insert all modules of a project in the modules tree.
         */
        void insert_modules();

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

    protected slots:
        /**
         * Confirm the selection of the selected module. It will accept() the dialog.
         *
         * @see QDialog.
         */
        void select_module();

        /**
         * Cancel the selection. It will reject() the dialog.
         * You will be unable to get a module from the get_selected_module() function.
         *
         * @see Dialog.
         */
        void cancel();

    private:
        Project_shptr project = nullptr;
        QVBoxLayout layout;
        Module_shptr selected_module = nullptr;

        // Modules part
        QTreeWidget modules_tree;
        std::map<QTreeWidgetItem*, ModuleItemCollection> modules_map;

        // Control part
        QHBoxLayout control_layout;
        QPushButton select_button;
        QPushButton cancel_button;

    };
}

#endif //__MODULESELECTIONDIALOG_H__
