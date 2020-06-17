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

#ifndef __TEMPLATEMATCHINGDIALOG_H__
#define __TEMPLATEMATCHINGDIALOG_H__

#include <Core/LogicModel/Gate/GateTemplate.h>
#include <Core/Matching/TemplateMatching.h>
#include <Core/LogicModel/LogicModel.h>
#include <GUI/Dialog/SelectGateTemplateDialog.h>
#include <GUI/Widget/DoubleSliderWidget.h>

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>

namespace degate
{
    /**
     * @class TemplateMatchingDialog
     * @brief Create a dialog to execute template matching on a specific area (can be project size).
     *
     * It will manage parameters dialog display and template matching run with a progress dialog.
     * All actions can be canceled.
     */
    class TemplateMatchingDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * Create a new template matching dialog.
         *
         * @param parent : the parent widget.
         * @param bounding_box : the bounding box of the area where to run the template matching.
         * @param project : the currently active project.
         * @param gate_templates : the list of gate templates to match.
         */
        TemplateMatchingDialog(QWidget* parent,
                               BoundingBox& bounding_box,
                               Project_shptr& project,
                               std::vector<GateTemplate_shptr>& gate_templates);

        ~TemplateMatchingDialog() override = default;

    protected slots:
        /**
         * Function called when the user push the "run" button. It will start template matching with selected
         * parameters.
         */
        void run();

    private:
        QVBoxLayout layout;
        QGridLayout content_layout;

        // Threshold hill climbing
        QLabel             hill_climbing_threshold_label;
        DoubleSliderWidget hill_climbing_threshold_edit;

        // Threshold
        QLabel             threshold_label;
        DoubleSliderWidget threshold_edit;

        // Image scale factor
        QLabel    image_scale_factor_label;
        QComboBox image_scale_factor_edit;

        // Max step
        QLabel   max_step_label;
        QSpinBox max_step_edit;

        // Template matching orientation(s)
        QLabel    orientations_label;
        QComboBox orientations_edit;

        // Template matching type
        QLabel    template_matching_type_label;
        QComboBox template_matching_type_edit;

        // Run button
        QHBoxLayout button_layout;
        QPushButton run_button;

        // Base objects for template matching
        BoundingBox                     bounding_box;
        Project_shptr                   project = nullptr;
        std::vector<GateTemplate_shptr> gate_templates;

    };
}

#endif //__TEMPLATEMATCHINGDIALOG_H__
