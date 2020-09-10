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

#include "ModuleSelectionDialog.h"

namespace degate
{

    ModuleSelectionDialog::ModuleSelectionDialog(Project_shptr project, QWidget* parent)
            : QDialog(parent), project(std::move(project))
    {
        setWindowFlags(Qt::Window);
        setWindowTitle(tr("Select module dialog"));

        // Modules part
        modules_tree.setColumnCount(2);
        modules_tree.setHeaderLabels(QStringList({
             tr("Module name"),
             tr("Module type")
         }));

        control_layout.addStretch(1);

        select_button.setText(tr("Select"));
        QObject::connect(&select_button, SIGNAL(clicked()), this, SLOT(select_module()));
        control_layout.addWidget(&select_button);

        cancel_button.setText(tr("Cancel"));
        QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(cancel()));
        control_layout.addWidget(&cancel_button);

        layout.addWidget(&modules_tree);
        layout.addLayout(&control_layout);

        setLayout(&layout);

        inset_modules();
    }

    Module_shptr ModuleSelectionDialog::get_selected_module()
    {
        return selected_module;
    }

    void ModuleSelectionDialog::inset_modules()
    {
        assert(project != nullptr);

        modules_tree.clear();
        modules_map.clear();

        Module_shptr module = project->get_logic_model()->get_main_module();

        auto item = new QTreeWidgetItem();
        modules_map[item] = { module, nullptr };
        item->setText(0, QString::fromStdString(project->get_name()));
        item->setText(1, QString::fromStdString(module->get_entity_name()));
        modules_tree.addTopLevelItem(item);

        for (auto iter = module->modules_begin(); iter != module->modules_end(); ++iter)
        {
            Module_shptr child_module = *iter;
            insert_module(item, child_module, module);
        }

        modules_tree.resizeColumnToContents(0);
        modules_tree.resizeColumnToContents(1);
        modules_tree.expandAll();
    }

    void ModuleSelectionDialog::insert_module(QTreeWidgetItem* parent_item, const Module_shptr& module, const Module_shptr& parent)
    {
        assert(project != nullptr);
        assert(parent_item != nullptr);
        assert(module != nullptr);
        assert(parent != nullptr);

        auto item = new QTreeWidgetItem();
        modules_map[item] = { module, parent };
        item->setText(0, QString::fromStdString(module->get_name()));
        item->setText(1, QString::fromStdString(module->get_entity_name()));
        parent_item->addChild(item);

        for (auto iter = module->modules_begin(); iter != module->modules_end(); ++iter)
        {
            Module_shptr child_module = *iter;
            insert_module(item, child_module, module);
        }
    }

    void ModuleSelectionDialog::select_module()
    {
        if (modules_tree.selectedItems().empty())
            return;

        selected_module = modules_map[modules_tree.selectedItems().at(0)].module;

        accept();
    }

    void ModuleSelectionDialog::cancel()
    {
        reject();
    }
}
