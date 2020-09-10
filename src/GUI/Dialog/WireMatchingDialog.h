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

#ifndef __WIREMATCHINGDIALOG_H__
#define __WIREMATCHINGDIALOG_H__

#include "Core/Project/Project.h"
#include "Core/LogicModel/LogicModel.h"
#include "GUI/Widget/DoubleSliderWidget.h"

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

namespace degate
{
    /**
     * @class WireMatchingDialog
     * @brief Create a dialog to execute wire matching on a specific area (can be project size).
     *
     * It will manage parameters dialog display and wire matching run with a progress dialog.
     * All actions can be canceled.
     */
    class WireMatchingDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * Create a new wire matching dialog.
         *
         * @param parent : the parent widget.
         * @param bounding_box : the bounding box of the area where to run the wire matching.
         * @param project : the currently active project.
         */
        WireMatchingDialog(QWidget* parent,
                           BoundingBox& bounding_box,
                           Project_shptr& project);

        ~WireMatchingDialog() override = default;

    protected slots:
        /**
         * Function called when the user push the "run" button. It will start wire matching with selected
         * parameters.
         */
        void run();

    private:
        QVBoxLayout layout;
        QGridLayout content_layout;

        // Wire diameter
        QLabel   wire_diameter_label;
        QSpinBox wire_diameter_edit;

        // Median filter width count
        QLabel   median_filter_width_label;
        QSpinBox median_filter_width_count_edit;

        // Sigma for gaussian blur
        QLabel             sigma_gaussian_blur_label;
        DoubleSliderWidget sigma_gaussian_blur_edit;

        // Minimum edge magnitude
        QLabel             min_edge_magnitude_label;
        DoubleSliderWidget min_edge_magnitude_edit;

        // Run button
        QHBoxLayout button_layout;
        QPushButton run_button;

        // Base objects for template matching
        BoundingBox   bounding_box;
        Project_shptr project = nullptr;
    };
}

#endif //__WIREMATCHINGDIALOG_H__
