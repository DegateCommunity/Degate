/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert

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

#include "RuleViolationsDialog.h"

namespace degate
{

    RuleViolationsTab::RuleViolationsTab(QWidget* parent)
            : QWidget(parent)
    {
        table.setColumnCount(4);

        table.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        QStringList list;
        list.append(tr("Layer"));
        list.append(tr("Class"));
        list.append(tr("Severity"));
        list.append(tr("Description"));
        table.setHorizontalHeaderLabels(list);
        table.resizeColumnsToContents();
        table.resizeRowsToContents();

        layout.addWidget(&table);
        setLayout(&layout);
    }

    void RuleViolationsTab::add_violation(RCViolation_shptr& violation)
    {
        table.insertRow(table.rowCount());

        // Layer
        auto layer_item = new QTableWidgetItem(QString::number(violation->get_object()->get_layer()->get_layer_pos()));
        layer_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table.setItem(table.rowCount() - 1, 0, layer_item);

        // Class
        auto class_item = new QTableWidgetItem(QString::fromStdString(violation->get_rc_violation_class()));
        class_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table.setItem(table.rowCount() - 1, 1, class_item);

        // Severity
        auto severity_item = new QTableWidgetItem(QString::fromStdString(violation->get_severity_as_string()));
        severity_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table.setItem(table.rowCount() - 1, 2, severity_item);

        // Description
        auto description_item = new QTableWidgetItem(QString::fromStdString(violation->get_problem_description()));
        description_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table.setItem(table.rowCount() - 1, 3, description_item);

        violations[table.rowCount() - 1] = violation;
    }

    void RuleViolationsTab::clear_violations()
    {
        table.clearContents();
        table.setRowCount(0);
        violations.clear();
    }

    std::vector<RCViolation_shptr> RuleViolationsTab::get_selection()
    {
        std::vector<RCViolation_shptr> res;

        auto selection = table.selectedItems();
        for (auto& e : selection)
        {
            res.push_back(violations[e->row()]);
        }

        return res;
    }

    void RuleViolationsTab::resize_to_content()
    {
        table.resizeColumnsToContents();
        table.resizeRowsToContents();
    }

    unsigned int RuleViolationsTab::get_violations_count()
    {
        return table.rowCount();
    }

    RuleViolationsDialog::RuleViolationsDialog(QWidget* parent, Project_shptr& project)
            : QDialog(nullptr), // nullptr to let the window "free" of parent's position constraints
              project(project),
              violations_tab(this),
              accepted_violations_tab(this)
    {
        setWindowTitle(tr("Rule violations"));
        setWindowIcon(QIcon(":/degate_logo.png"));

        // Tabs
        tabs.addTab(&violations_tab, tr("Violations"));
        tabs.addTab(&accepted_violations_tab, tr("Accepted violations"));

        // Violations count
        violations_count_label.setText(tr("Number of violations: %1").arg(0));
        control_layout.addWidget(&violations_count_label, 0, 0);

        // Refresh
        refresh_button.setText(tr("Refresh"));
        QObject::connect(&refresh_button, SIGNAL(clicked()), this, SLOT(run_checks()));
        control_layout.addWidget(&refresh_button, 1, 0);

        // Violations action button
        violations_action_button.setText(tr("Accept selected violation(s)"));
        QObject::connect(&violations_action_button, SIGNAL(clicked()), this, SLOT(selected_violations_action()));
        control_layout.addWidget(&violations_action_button, 1, 1);

        // Filter
        filter_label.setText(tr("Filter:"));
        filter_reset_button.setText(tr("Reset"));
        control_layout.addWidget(&filter_label, 1, 2);
        control_layout.addWidget(&filter_edit, 1, 3);
        control_layout.addWidget(&filter_reset_button, 1, 4);

        // Goto button
        jump_to_button.setText(tr("Goto selected object"));
        control_layout.addWidget(&jump_to_button, 2, 0);
        QObject::connect(&jump_to_button, SIGNAL(clicked()), this, SLOT(goto_button_clicked()));

        // Reset filter
        QObject::connect(&filter_reset_button, SIGNAL(clicked()), this, SLOT(reset_filter()));

        // Apply filter on return pressed (another solution is to rerun check for every filter change)
        QObject::connect(&filter_edit, SIGNAL(returnPressed()), this, SLOT(run_checks()));

        // Selected tab change
        QObject::connect(&tabs, SIGNAL(currentChanged(int)), this, SLOT(selected_tab_changed(int)));

        // Control layout stretch
        control_layout.setRowStretch(0, 1);
        control_layout.setRowStretch(1, 1);
        control_layout.setRowStretch(2, 1);

        layout.addWidget(&tabs, 0, 0);
        layout.addLayout(&control_layout, 1, 0);
        layout.setRowStretch(0, 1);

        setLayout(&layout);

        if (parent != nullptr)
            resize(parent->size() * 0.7);
    }

    RuleViolationsDialog::~RuleViolationsDialog()
    {
        tabs.blockSignals(true);
    }

    std::string RuleViolationsDialog::get_filter_text()
    {
        return filter_edit.text().toStdString();
    }

    void RuleViolationsDialog::run_checks()
    {
        assert(project != nullptr);

        accepted_violations_tab.clear_violations();
        violations_tab.clear_violations();

        rule_checker.run(project->get_logic_model());
        RCVContainer const& violations = rule_checker.get_rc_violations();

        for (auto& v : violations)
        {
            // Check if violation is blacklisted
            if (v != nullptr && !project->get_rcv_blacklist().contains(v) && v->matches_filter(get_filter_text()))
            {
                violations_tab.add_violation(const_cast<RCViolation_shptr&>(v));
            }
        }

        RCVContainer remove_from_blacklist;

        for (auto& v : project->get_rcv_blacklist())
        {
            // Check if violation is blacklisted
            if (violations.contains(v))
            {
                if (v->matches_filter(get_filter_text()))
                {
                    accepted_violations_tab.add_violation(v);
                }
            }
            else
            {
                remove_from_blacklist.push_back(v);
            }
        }

        for (auto& v : remove_from_blacklist)
        {
            project->get_rcv_blacklist().erase(v);
        }

        violations_tab.resize_to_content();
        accepted_violations_tab.resize_to_content();

        violations_count_label.setText(tr("Number of violations : %1").arg(violations_tab.get_violations_count()));
    }

    void RuleViolationsDialog::selected_violations_action()
    {
        if (tabs.currentIndex() == 0)
        {
            for (auto& e : violations_tab.get_selection())
            {
                if (!project->get_rcv_blacklist().contains(e))
                    project->get_rcv_blacklist().push_back(e);
            }
        }
        else if (tabs.currentIndex() == 1)
        {
            for (auto& e : accepted_violations_tab.get_selection())
            {
                if (project->get_rcv_blacklist().contains(e))
                    project->get_rcv_blacklist().erase(e);
            }
        }

        run_checks();
    }

    void RuleViolationsDialog::selected_tab_changed(int tab_index)
    {
        if (tab_index == 0)
            violations_action_button.setText(tr("Accept selected violation(s)"));
        else if (tab_index == 1)
            violations_action_button.setText(tr("Reject selected violation(s)"));
    }

    void RuleViolationsDialog::reset_filter()
    {
        filter_edit.setText("");
        run_checks();
    }

    void RuleViolationsDialog::goto_button_clicked()
    {
        PlacedLogicModelObject_shptr object = nullptr;

        if (tabs.currentIndex() == 0)
        {
            object = violations_tab.get_selection().front()->get_object();
        }
        else if (tabs.currentIndex() == 1)
        {
            object = accepted_violations_tab.get_selection().front()->get_object();
        }

        if (object != nullptr)
            emit goto_object(object);
    }
}
