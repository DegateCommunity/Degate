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

#ifndef __RULEVIOLATIONSDIALOG_H__
#define __RULEVIOLATIONSDIALOG_H__

#include "Core/RuleCheck/RCViolation.h"
#include "Core/Project/Project.h"
#include "Core/RuleCheck/RuleChecker.h"

#include <map>
#include <QDialog>
#include <QWidget>
#include <QTabWidget>
#include <QScrollArea>
#include <QTableWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

namespace degate
{
    /**
     * @class RuleViolationsList
     * @brief Base widget for listing rule violations.
     */
    class RuleViolationsTab : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * Create a new rule violations tab.
         *
         * @param parent : the parent of the widget.
         */
        explicit RuleViolationsTab(QWidget* parent);
        ~RuleViolationsTab() override = default;

        /**
         * Add a new violation to the rule violations list.
         * It will be added at the end of the list.
         *
         * @param violation : the violation to add.
         */
        void add_violation(RCViolation_shptr& violation);

        /**
         * Clear/delete all violations of the rule violations list.
         */
        void clear_violations();

        /**
         * Get the selected rule violations.
         *
         * @return Returns a vector of selected rule violations.
         */
        std::vector<RCViolation_shptr> get_selection();

        /**
         * Resize the list and columns to the content.
         */
        void resize_to_content();

        /**
         * Get the number of violations of the tab.
         *
         * @return Returns the number of violations.
         */
        unsigned int get_violations_count();

    private:
        QGridLayout layout;
        QTableWidget table;

        std::map<int, RCViolation_shptr> violations;
    };

    /**
     * @class RuleViolationsDialog
     * @brief Dialog that lists all rule violations and accepted rule violations.
     *
     * The dialog should be open as modeless (QDialog::show) to let the "goto" function work.
     * Even if the parent is set, it will not link it with the QDialog. It will only be used for setting the dialog size.
     */
    class RuleViolationsDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * Create a new rule violations dialog.
         *
         * @param parent : the parent of the dialog.
         * @param project : the current project.
         */
        RuleViolationsDialog(QWidget* parent, Project_shptr& project);
        ~RuleViolationsDialog() override;

        /**
         * Get the filter text.
         *
         * @return Returns the filter text as std::string.
         */
        std::string get_filter_text();

    public slots:
        /**
         * Run rule checks on the project logic model.
         * It can be an expensive operation regarding the project size.
         */
        void run_checks();

        /**
         * Accept or reject selected violations regarding the currently active tab.
         */
        void selected_violations_action();

        /**
         * Called when the selected tab changed.
         * It will change the violations_action_button text ("accept" or "reject" selected violations).
         *
         * @param tab_index : the current selected tab index.
         */
        void selected_tab_changed(int tab_index);

        /**
         * Reset the filter text and rerun checks.
         */
        void reset_filter();

    protected slots:
        /**
         * Called when the goto button is clicked.
         */
        void goto_button_clicked();

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
        QTabWidget tabs;

        // Tabs
        RuleViolationsTab violations_tab;
        RuleViolationsTab accepted_violations_tab;

        // Rule checker
        RuleChecker rule_checker;

        // Control layout
        QGridLayout control_layout;
        QLabel violations_count_label;
        QPushButton violations_action_button;
        QPushButton refresh_button;

        // Filter
        QLabel filter_label;
        QLineEdit filter_edit;
        QPushButton filter_reset_button;

        // Jump/Go to button
        QPushButton jump_to_button;

    };
}

#endif //__RULEVIOLATIONSDIALOG_H__
