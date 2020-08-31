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

#include "TemplateMatchingDialog.h"

#include "Core/LogicModel/LogicModelHelper.h"
#include "GUI/Dialog/ProgressDialog.h"

#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

namespace degate
{

    TemplateMatchingDialog::TemplateMatchingDialog(QWidget* parent,
                                                   BoundingBox& bounding_box,
                                                   Project_shptr& project,
                                                   std::vector<GateTemplate_shptr>& gate_templates)
            : QDialog(parent),
              bounding_box(bounding_box),
              project(project),
              gate_templates(gate_templates)
    {
        setWindowTitle(tr("Template matching"));
        setWindowFlags(Qt::Window);

        // Threshold to start hill climbing
        hill_climbing_threshold_label.setText(tr("Threshold to start hill climbing:"));
        hill_climbing_threshold_edit.set_minimum(0);
        hill_climbing_threshold_edit.set_maximum(1);
        hill_climbing_threshold_edit.set_single_step(0.01);
        hill_climbing_threshold_edit.set_decimals(2);
        hill_climbing_threshold_edit.set_value(0.40);
        content_layout.addWidget(&hill_climbing_threshold_label, 0, 0);
        content_layout.addWidget(&hill_climbing_threshold_edit, 0, 1);

        // Threshold for detection
        threshold_label.setText(tr("Threshold for detection:"));
        threshold_edit.set_minimum(0);
        threshold_edit.set_maximum(1);
        threshold_edit.set_single_step(0.01);
        threshold_edit.set_decimals(2);
        threshold_edit.set_value(0.70);
        content_layout.addWidget(&threshold_label, 1, 0);
        content_layout.addWidget(&threshold_edit, 1, 1);

        // Scale image down by factor
        image_scale_factor_label.setText(tr("Scale image down by factor:"));
        ScalingManager_shptr scaling_manager = project->get_logic_model()->get_current_layer()->get_scaling_manager();

        if (scaling_manager == nullptr)
            return;

        const auto steps = scaling_manager->get_zoom_steps();
        for (auto& step : steps)
        {
            bool is_ok = true;
            if (step != 1)
            {
                for (auto& gate : gate_templates)
                {
                    if (std::floor(static_cast<double>(gate->get_width()) / static_cast<double>(step)) < 10 ||
                        std::floor(static_cast<double>(gate->get_height()) / static_cast<double>(step) < 10))
                    {
                        is_ok = false;
                    }
                }
            }

            if (is_ok)
                image_scale_factor_edit.addItem(QString::number(step));
        }


        content_layout.addWidget(&image_scale_factor_label, 2, 0);
        content_layout.addWidget(&image_scale_factor_edit, 2, 1);

        // Max step size for search after scaling
        max_step_label.setText(tr("Max step size for search after scaling (in pixel):"));
        max_step_edit.setMinimum(0);
        max_step_edit.setValue(3);
        content_layout.addWidget(&max_step_label, 3, 0);
        content_layout.addWidget(&max_step_edit, 3, 1);

        // Match template orientation(s)
        orientations_label.setText(tr("Match template orientation(s):"));
        orientations_edit.addItem(tr("Any"), 1);
        orientations_edit.addItem(tr("Normal"), 2);
        orientations_edit.addItem(tr("Flipped left-right"), 3);
        orientations_edit.addItem(tr("Flipped up-down"), 4);
        orientations_edit.addItem(tr("Flipped both"), 5);

        content_layout.addWidget(&orientations_label, 4, 0);
        content_layout.addWidget(&orientations_edit, 4, 1);

        // Match template type
        template_matching_type_label.setText(tr("Match template type:"));
        template_matching_type_edit.addItem(tr("Default"), 1);
        template_matching_type_edit.addItem(tr("Along grid in rows"), 2);
        template_matching_type_edit.addItem(tr("Along grid in columns"), 3);

        content_layout.addWidget(&template_matching_type_label, 5, 0);
        content_layout.addWidget(&template_matching_type_edit, 5, 1);

        // Button
        run_button.setText("Run");
        QObject::connect(&run_button, SIGNAL(clicked()), this, SLOT(run()));

        // Layouts
        button_layout.addStretch(1);
        button_layout.addWidget(&run_button);

        layout.addLayout(&content_layout);
        layout.addSpacing(5);
        layout.addLayout(&button_layout);

        setLayout(&layout);
    }

    void TemplateMatchingDialog::run()
    {
        // Check parameters
        if (gate_templates.empty())
        {
            QMessageBox::warning(this,
                                 tr("Warning"),
                                 tr("There is no gate template selected for template matching."));
            reject();
            return;
        }

        // Check every selected gate template size
        for (auto& e : gate_templates)
        {
            if (e->get_width() >= bounding_box.get_width() || e->get_height() >= bounding_box.get_height())
            {
                QMessageBox::warning(this,
                                     tr("Warning"),
                                     tr("One of the selected gate templates is larger than the search area."));
                reject();
                return;
            }
        }

        // Create the matching object
        TemplateMatching_shptr matching = nullptr;

        auto template_matching_type = template_matching_type_edit.currentData();
        if (template_matching_type == 1)
        {
            matching = std::make_shared<TemplateMatchingNormal>();
        }
        else if (template_matching_type == 2)
        {
            matching = std::make_shared<TemplateMatchingInRows>();
        }
        else if (template_matching_type == 3)
        {
            matching = std::make_shared<TemplateMatchingInCols>();
        }

        // Set parameters
        matching->set_threshold_hc(hill_climbing_threshold_edit.get_value());
        matching->set_threshold_detection(threshold_edit.get_value());
        matching->set_max_step_size(max_step_edit.value());
        matching->set_scaling_factor(image_scale_factor_edit.currentText()
                                                            .toUInt());
        matching->set_templates(std::list<GateTemplate_shptr>(gate_templates.begin(), gate_templates.end()));
        matching->set_layers(project->get_logic_model()
                                    ->get_current_layer(),
                             get_first_logic_layer(project->get_logic_model()));

        // Set orientations
        std::list<degate::Gate::ORIENTATION> orientations_list;

        auto orientation_string = orientations_edit.currentData();
        if (orientation_string == 1) // any
        {
            orientations_list.push_back(Gate::ORIENTATION_NORMAL);
            orientations_list.push_back(Gate::ORIENTATION_FLIPPED_UP_DOWN);
            orientations_list.push_back(Gate::ORIENTATION_FLIPPED_LEFT_RIGHT);
            orientations_list.push_back(Gate::ORIENTATION_FLIPPED_BOTH);
        }
        else if (orientation_string == 2) // normal
        {
            orientations_list.push_back(Gate::ORIENTATION_NORMAL);
        }
        else if (orientation_string == 3) // flipped left right
        {
            orientations_list.push_back(Gate::ORIENTATION_FLIPPED_LEFT_RIGHT);
        }
        else if (orientation_string == 4) // flipped up down
        {
            orientations_list.push_back(Gate::ORIENTATION_FLIPPED_UP_DOWN);
        }
        else if (orientation_string == 5) // flipped both
        {
            orientations_list.push_back(Gate::ORIENTATION_FLIPPED_BOTH);
        }
        matching->set_orientations(orientations_list);

        // Start progress dialog
        ProgressDialog progress_dialog(tr("Template matching"),
                                       matching,
                                       this->parentWidget());

        // Set the job to start the template matching (will run in another thread)
        progress_dialog.set_job([matching, this]()
                                {
                                    matching->init(bounding_box, project);
                                    matching->run();
                                });

        // Close this dialog
        accept();

        // Start the process
        progress_dialog.exec();

        if (progress_dialog.was_canceled())
            return;

        // If the template matching succeeded, show statistics (number of hits)
        QMessageBox::information(this->parentWidget(),
                                 tr("Template matching results"),
                                 tr("Found %1 match(es).").arg(matching->get_number_of_hits()));
    }
}