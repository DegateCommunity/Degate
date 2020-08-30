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

#include "ViaMatchingDialog.h"

#include <GUI/Dialog/ProgressDialog.h>
#include <Core/Matching/ViaMatching.h>
#include <QtWidgets/QMessageBox>

namespace degate
{

    ViaMatchingDialog::ViaMatchingDialog(QWidget* parent, BoundingBox& bounding_box, Project_shptr& project)
            : QDialog(parent), bounding_box(bounding_box), project(project)
    {
        setWindowTitle(tr("Via matching"));
        setWindowFlags(Qt::Window);

        // Threshold for detection
        threshold_label.setText(tr("Threshold for detection:"));
        threshold_edit.set_minimum(0);
        threshold_edit.set_maximum(1);
        threshold_edit.set_single_step(0.01);
        threshold_edit.set_decimals(2);
        threshold_edit.set_value(0.95);
        content_layout.addWidget(&threshold_label, 0, 0);
        content_layout.addWidget(&threshold_edit, 0, 1);

        // Via diameter
        via_diameter_label.setText(tr("Via diameter:"));
        via_diameter_edit.setMinimum(0);
        via_diameter_edit.setValue(project->get_default_via_diameter());
        content_layout.addWidget(&via_diameter_label, 1, 0);
        content_layout.addWidget(&via_diameter_edit, 1, 1);

        // Via count
        vias_count_label.setText(tr("Max vias to match (0 = all):"));
        vias_count_edit.setMinimum(0);
        vias_count_edit.setValue(0);
        content_layout.addWidget(&vias_count_label, 2, 0);
        content_layout.addWidget(&vias_count_edit, 2, 1);

        // Button
        run_button.setText(tr("Run"));
        QObject::connect(&run_button, SIGNAL(clicked()), this, SLOT(run()));

        // Layouts
        button_layout.addStretch(1);
        button_layout.addWidget(&run_button);

        layout.addLayout(&content_layout);
        layout.addSpacing(5);
        layout.addLayout(&button_layout);

        setLayout(&layout);
    }

    void ViaMatchingDialog::run()
    {
        auto via_matching = std::make_shared<ViaMatching>();

        via_matching->set_diameter(via_diameter_edit.value());
        via_matching->set_merge_n_vias(vias_count_edit.value());
        via_matching->set_threshold_match(threshold_edit.get_value());

        // Start progress dialog
        ProgressDialog progress_dialog(tr("Via matching"),
                                       via_matching,
                                       this->parentWidget());

        // Set the job to start the via matching (will run in another thread)
        progress_dialog.set_job([via_matching, this]()
                                {
                                    via_matching->init(bounding_box, project);
                                    via_matching->run();
                                });

        // Close this dialog
        accept();

        // Start the process
        progress_dialog.exec();

        if (progress_dialog.was_canceled())
            return;

        QMessageBox::information(this->parentWidget(),
                                 tr("Via matching results"),
                                 tr("Via matching just finished."));

    }
}
