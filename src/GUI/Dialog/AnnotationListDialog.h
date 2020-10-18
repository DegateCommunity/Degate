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

#ifndef __ANNOTATIONLISTDIALOG_H__
#define __ANNOTATIONLISTDIALOG_H__

#include "Core/Project/Project.h"

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>

#include <map>

namespace degate
{
    /**
     * @class AnnotationListDialog
     * @brief A dialog that lists all annotations of a project.
     */
    class AnnotationListDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Create an annotation list dialog.
         *
         * @param parent : the parent widget.
         * @param project : the current project.
         */
        AnnotationListDialog(QWidget* parent, Project_shptr project);
        ~AnnotationListDialog() override = default;

    protected slots:
        /**
         * Update the annotation list.
         */
        void update_annotation_list();

        /**
         * Reset the filter text and rerun checks.
         */
        void reset_filter();

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

        // Table
        QTableWidget table;

        // Control layout
        QGridLayout control_layout;
        QLabel filter_label;
        QLineEdit filter_edit;
        QPushButton filter_reset_button;

        // Jump/Go to button
        QPushButton jump_to_button;

        std::map<int, Annotation_shptr> annotations;
    };
}

#endif //__ANNOTATIONLISTDIALOG_H__
