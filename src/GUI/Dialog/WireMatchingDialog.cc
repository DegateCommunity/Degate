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

#include "WireMatchingDialog.h"

#include "GUI/Dialog/ProgressDialog.h"
#include "Core/Matching/WireMatching.h"
#include <QtWidgets/QMessageBox>

namespace degate
{

    WireMatchingDialog::WireMatchingDialog(QWidget* parent, BoundingBox& bounding_box, Project_shptr& project)
            : QDialog(parent), bounding_box(bounding_box), project(project)
    {
        setWindowTitle(tr("Wire matching"));
        setWindowFlags(Qt::Window);

        // Via diameter
        wire_diameter_label.setText(tr("Wire diameter:"));
        wire_diameter_edit.setMinimum(0);
        wire_diameter_edit.setValue(4);
        content_layout.addWidget(&wire_diameter_label, 0, 0);
        content_layout.addWidget(&wire_diameter_edit, 0, 1);

        // Median filter width count
        median_filter_width_label.setText(tr("Median filter width count (0 = disable):"));
        median_filter_width_count_edit.setMinimum(0);
        median_filter_width_count_edit.setValue(3);
        content_layout.addWidget(&median_filter_width_label, 1, 0);
        content_layout.addWidget(&median_filter_width_count_edit, 1, 1);

        // Sigma for gaussian blur
        sigma_gaussian_blur_label.setText(tr("Sigma for gaussian blur (0 = disable):"));
        sigma_gaussian_blur_edit.set_minimum(0);
        sigma_gaussian_blur_edit.set_maximum(1);
        sigma_gaussian_blur_edit.set_single_step(0.01);
        sigma_gaussian_blur_edit.set_decimals(2);
        sigma_gaussian_blur_edit.set_value(0.5);
        content_layout.addWidget(&sigma_gaussian_blur_label, 2, 0);
        content_layout.addWidget(&sigma_gaussian_blur_edit, 2, 1);

        // Minimum edge magnitude
        min_edge_magnitude_label.setText(tr("Minimum edge magnitude:"));
        min_edge_magnitude_edit.set_minimum(0);
        min_edge_magnitude_edit.set_maximum(1);
        min_edge_magnitude_edit.set_single_step(0.01);
        min_edge_magnitude_edit.set_decimals(2);
        min_edge_magnitude_edit.set_value(0.25);
        content_layout.addWidget(&min_edge_magnitude_label, 3, 0);
        content_layout.addWidget(&min_edge_magnitude_edit, 3, 1);

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

    void WireMatchingDialog::run()
    {
        auto wire_matching = std::make_shared<WireMatching>();

        wire_matching->set_wire_diameter(wire_diameter_edit.value());
        wire_matching->set_median_filter_width(median_filter_width_count_edit.value());
        wire_matching->set_sigma(sigma_gaussian_blur_edit.get_value());
        wire_matching->set_min_edge_magnitude(min_edge_magnitude_edit.get_value());

        // Start progress dialog
        ProgressDialog progress_dialog(tr("Wire matching"),
                                       wire_matching,
                                       this->parentWidget());

        // Set the job to start the via matching (will run in another thread)
        progress_dialog.set_job([wire_matching, this]()
                                {
                                    wire_matching->init(bounding_box, project);
                                    wire_matching->run();
                                });

        // Close this dialog
        accept();

        // Start the process
        progress_dialog.exec();

        if (progress_dialog.was_canceled())
            return;

        QMessageBox::information(this->parentWidget(),
                                 tr("Wire matching results"),
                                 tr("Wire matching just finished."));
    }
}
