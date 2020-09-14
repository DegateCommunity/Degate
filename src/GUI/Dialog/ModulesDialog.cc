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

#include "ModulesDialog.h"
#include "Core/Generator/VerilogModuleGenerator.h"
#include "Core/Utils/DegateHelper.h"

#include <utility>
#include <QFileDialog>

namespace degate
{
    ModulesDialog::ModulesDialog(QWidget* parent, Project_shptr project)
            : QDialog(parent), project(std::move(project))
    {
        setWindowFlags(Qt::Window);
        setWindowTitle(tr("Modules"));

        // Modules part
        modules_tree.setColumnCount(2);
        modules_tree.setHeaderLabels(QStringList({
            tr("Module name"),
            tr("Module type")
        }));
        layout.addWidget(&modules_tree, 0, 0);
        QObject::connect(&modules_tree, SIGNAL(itemSelectionChanged()), this, SLOT(on_module_selection_changed()));
        QObject::connect(&modules_tree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(on_module_changed(QTreeWidgetItem*, int)));

        // Gates part
        gates_tree.setColumnCount(2);
        gates_tree.setHeaderLabels(QStringList({
            tr("Gate name"),
            tr("Gate type")}
            ));
        layout.addWidget(&gates_tree, 0, 1);
        QObject::connect(&gates_tree, SIGNAL(itemSelectionChanged()), this, SLOT(on_gate_selection_changed()));

        // Ports part
        ports_tree.setColumnCount(3);
        ports_tree.setHeaderLabels(QStringList({
            tr("Module port name"),
            tr("Gate port"),
            tr("Gate")
        }));
        layout.addWidget(&ports_tree, 0, 2);
        QObject::connect(&ports_tree, SIGNAL(itemSelectionChanged()), this, SLOT(on_port_selection_changed()));
        QObject::connect(&ports_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(on_port_double_clicked(QTreeWidgetItem*, int)));
        QObject::connect(&ports_tree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(on_port_changed(QTreeWidgetItem*, int)));

        // Control part
        add_module_button.setText(tr("Add module"));
        add_module_button.setDisabled(true);
        QObject::connect(&add_module_button, SIGNAL(clicked()), this, SLOT(add_module()));

        remove_module_button.setText(tr("Remove module"));
        remove_module_button.setDisabled(true);
        QObject::connect(&remove_module_button, SIGNAL(clicked()), this, SLOT(remove_module()));

        determine_ports_module_button.setText(tr("Determine ports module"));
        determine_ports_module_button.setDisabled(true);
        QObject::connect(&determine_ports_module_button, SIGNAL(clicked()), this, SLOT(determine_module_ports()));

        export_module_button.setText(tr("Export module"));
        export_module_button.setDisabled(true);
        QObject::connect(&export_module_button, SIGNAL(clicked()), this, SLOT(export_module()));


        move_gate_into_module_button.setText(tr("Move gate into module"));
        move_gate_into_module_button.setDisabled(true);
        QObject::connect(&move_gate_into_module_button, SIGNAL(clicked()), this, SLOT(move_gate_into_module()));

        jump_to_gate_button.setText(tr("Jump to gate"));
        jump_to_gate_button.setDisabled(true);
        QObject::connect(&jump_to_gate_button, SIGNAL(clicked()), this, SLOT(jump_to_gate()));


        jump_to_port_button.setText(tr("Jump to port"));
        jump_to_port_button.setDisabled(true);
        QObject::connect(&jump_to_port_button, SIGNAL(clicked()), this, SLOT(jump_to_port()));


        close_button.setText(tr("Close"));
        QObject::connect(&close_button, SIGNAL(clicked()), this, SLOT(accept()));

        layout.addWidget(&add_module_button, 1, 0);
        layout.addWidget(&remove_module_button, 2, 0);
        layout.addWidget(&determine_ports_module_button, 3, 0);
        layout.addWidget(&export_module_button, 4, 0);

        layout.addWidget(&jump_to_gate_button, 1, 1);
        layout.addWidget(&move_gate_into_module_button, 2, 1);

        layout.addWidget(&jump_to_port_button, 1, 2);

        layout.addWidget(&close_button, 5, 2);

        setLayout(&layout);

        inset_modules();

        // Size
        if (parent != nullptr)
            resize(parent->size() * 0.5);
    }

    void ModulesDialog::reload()
    {
        if (project == nullptr)
            return;

        inset_modules();
    }

    void ModulesDialog::inset_modules()
    {
        assert(project != nullptr);

        modules_tree.clear();
        modules_map.clear();

        gates_tree.clear();
        gates_map.clear();

        ports_tree.clear();
        ports_map.clear();

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

    void ModulesDialog::insert_module(QTreeWidgetItem* parent_item, const Module_shptr& module, const Module_shptr& parent)
    {
        assert(project != nullptr);
        assert(parent_item != nullptr);
        assert(module != nullptr);
        assert(parent != nullptr);

        auto item = new QTreeWidgetItem();
        modules_map[item] = { module, parent };
        item->setText(0, QString::fromStdString(module->get_name()));
        item->setText(1, QString::fromStdString(module->get_entity_name()));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        parent_item->addChild(item);

        for (auto iter = module->modules_begin(); iter != module->modules_end(); ++iter)
        {
            Module_shptr child_module = *iter;
            insert_module(item, child_module, module);
        }
    }

    void ModulesDialog::insert_gates(const Module_shptr& module)
    {
        assert(project != nullptr);
        assert(module != nullptr);

        gates_tree.clear();
        gates_map.clear();

        for (auto iter = module->gates_begin(); iter != module->gates_end(); ++iter)
        {
            Gate_shptr gate = *iter;

            auto item = new QTreeWidgetItem();
            gates_map[item] = gate;
            item->setText(0, QString::fromStdString(gate->get_descriptive_identifier()));
            item->setText(1, QString::fromStdString(gate->get_gate_template()->get_name()));
            gates_tree.addTopLevelItem(item);
        }
    }

    void ModulesDialog::insert_ports(const Module_shptr& module)
    {
        assert(project != nullptr);
        assert(module != nullptr);

        ports_tree.clear();
        ports_map.clear();

        std::vector<std::string> module_port_deletion_list;

        for (auto iter = module->ports_begin(); iter != module->ports_end(); ++iter)
        {
            std::string mod_port_name = iter->first;
            GatePort_shptr gp = iter->second;

            Gate_shptr gate = gp->get_gate();

            // If the gate was deleted, delete the module port.
            if (gate == nullptr)
            {
                module_port_deletion_list.push_back(mod_port_name);
                continue;
            }

            auto item = new QTreeWidgetItem();
            ports_map[item] = gp;
            item->setText(0, QString::fromStdString(mod_port_name));
            item->setText(1, QString::fromStdString(gp->get_descriptive_identifier()));
            item->setText(2, QString::fromStdString(gate->get_descriptive_identifier()));
            ports_tree.addTopLevelItem(item);
        }

        for (auto& e : module_port_deletion_list)
            module->remove_port(e);
    }

    void ModulesDialog::on_module_selection_changed()
    {
        if (modules_tree.selectedItems().empty())
        {
            add_module_button.setDisabled(true);
            remove_module_button.setDisabled(true);
            determine_ports_module_button.setDisabled(true);
            export_module_button.setDisabled(true);

            return;
        }

        add_module_button.setDisabled(false);
        remove_module_button.setDisabled(false);
        determine_ports_module_button.setDisabled(false);
        export_module_button.setDisabled(false);

        auto module_collection = modules_map[modules_tree.selectedItems().at(0)];

        insert_gates(module_collection.module);
        insert_ports(module_collection.module);

        gates_tree.resizeColumnToContents(0);
        gates_tree.resizeColumnToContents(1);

        ports_tree.resizeColumnToContents(0);
        ports_tree.resizeColumnToContents(1);
        ports_tree.resizeColumnToContents(2);
    }

    void ModulesDialog::on_gate_selection_changed()
    {
        if (gates_tree.selectedItems().empty())
        {
            move_gate_into_module_button.setDisabled(true);
            jump_to_gate_button.setDisabled(true);

            return;
        }

        move_gate_into_module_button.setDisabled(false);
        jump_to_gate_button.setDisabled(false);
    }

    void ModulesDialog::on_port_selection_changed()
    {
        if (ports_tree.selectedItems().empty())
        {
            jump_to_port_button.setDisabled(true);

            return;
        }

        jump_to_port_button.setDisabled(false);
    }

    void ModulesDialog::add_module()
    {
        if (modules_tree.selectedItems().empty())
            return;

        auto module = modules_map[modules_tree.selectedItems().at(0)].module;

        Module_shptr new_module(new Module(tr("Click to edit").toStdString()));
        module->add_module(new_module);

        insert_module(modules_tree.selectedItems().at(0), new_module, module);
    }

    void ModulesDialog::remove_module()
    {
        if (modules_tree.selectedItems().empty())
            return;

        auto module_collection = modules_map[modules_tree.selectedItems().at(0)];

        if (module_collection.module->is_main_module())
            return;

        module_collection.parent->remove_module(module_collection.module);

        inset_modules();
    }

    void ModulesDialog::determine_module_ports()
    {
        if (modules_tree.selectedItems().empty())
            return;

        auto module_collection = modules_map[modules_tree.selectedItems().at(0)];

        if (module_collection.module->is_main_module())
            determine_module_ports_for_root(project->get_logic_model());
        else
            module_collection.module->determine_module_ports();

        insert_ports(module_collection.module);
    }

    void ModulesDialog::export_module()
    {
        if (modules_tree.selectedItems().empty())
            return;

        QString dir = QFileDialog::getExistingDirectory(this, tr("Select the directory where to export the module"));

        if (dir.isNull())
            return;

        auto module = modules_map[modules_tree.selectedItems().at(0)].module;

        std::string filename;
        if (!module->is_main_module())
            filename = (!module->get_entity_name().empty() ? module->get_entity_name() : module->get_name()) + ".v";
        else
            filename = project->get_name() + ".v";

        std::string path = join_pathes(dir.toStdString(), filename);

        VerilogModuleGenerator code_generator(module);
        std::string impl = code_generator.generate();
        write_string_to_file(path, impl);
    }

    void ModulesDialog::move_gate_into_module()
    {
        if (gates_tree.selectedItems().empty())
            return;

        ModuleSelectionDialog dialog(project, this);
        auto ret = dialog.exec();

        if (ret != QDialog::Accepted)
            return;

        auto selected_module = dialog.get_selected_module();

        if (selected_module == nullptr)
            return;

        auto selected_gate = gates_map[gates_tree.selectedItems().at(0)];
        auto old_module = modules_map[modules_tree.selectedItems().at(0)].module;

        old_module->remove_gate(selected_gate);
        selected_module->add_gate(selected_gate);

        insert_gates(old_module);
        insert_ports(old_module);
    }

    void ModulesDialog::jump_to_gate()
    {
        if (gates_tree.selectedItems().empty())
            return;

        PlacedLogicModelObject_shptr object = gates_map[gates_tree.selectedItems().at(0)];

        if(object != nullptr)
            emit goto_object(object);
    }

    void ModulesDialog::jump_to_port()
    {
        if (ports_tree.selectedItems().empty())
            return;

        PlacedLogicModelObject_shptr object = ports_map[ports_tree.selectedItems().at(0)];

        if(object != nullptr)
            emit goto_object(object);
    }

    void ModulesDialog::on_module_changed(QTreeWidgetItem* item, int column)
    {
        if (item == nullptr)
            return;

        auto module = modules_map[item].module;

        module->set_name(item->text(0).toStdString());
        module->set_entity_name(item->text(1).toStdString());
    }

    void ModulesDialog::on_port_double_clicked(QTreeWidgetItem* item, int column)
    {
        // Trick to allow edit only for a specific column.
        if (column == 0)
        {
            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ports_tree.editItem(item, column);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
    }

    void ModulesDialog::on_port_changed(QTreeWidgetItem* item, int column)
    {
        if (item == nullptr || modules_tree.selectedItems().empty())
            return;

        auto port = ports_map[item];
        auto module = modules_map[modules_tree.selectedItems().at(0)].module;

        module->set_module_port_name(item->text(0).toStdString(), port);
    }
}
