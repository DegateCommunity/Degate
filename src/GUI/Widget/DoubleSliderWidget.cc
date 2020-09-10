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

#include "DoubleSliderWidget.h"

#include <iostream>
#include <cmath>

namespace degate
{

    DoubleSliderWidget::DoubleSliderWidget(QWidget *parent)
            : QWidget(parent)
    {
        slider.setOrientation(Qt::Horizontal);
        slider.setSingleStep(1);

        set_single_step(single_step);
        set_minimum(minimum);
        set_maximum(maximum);

        spin_box.setValue(minimum);

        layout.addWidget(&slider);
        layout.addWidget(&spin_box);

        setLayout(&layout);

        QObject::connect(&slider, SIGNAL(valueChanged(int)),this, SLOT(notify_slider_value_changed(int)));
        QObject::connect(&spin_box, SIGNAL(valueChanged(double)),this, SLOT(notify_spin_box_value_changed(double)));
    }

    void DoubleSliderWidget::set_single_step(double single_step)
    {
        assert(single_step != 0);

        this->single_step = single_step;

        update_range();

        spin_box.setSingleStep(single_step);
    }

    double DoubleSliderWidget::get_single_step()
    {
        return single_step;
    }

    void DoubleSliderWidget::set_minimum(double minimum)
    {
        this->minimum = minimum;
        update_range();
    }

    double DoubleSliderWidget::get_minimum()
    {
        return minimum;
    }

    void DoubleSliderWidget::set_maximum(double maximum)
    {
        this->maximum = maximum;
        update_range();
    }

    double DoubleSliderWidget::get_maximum()
    {
        return maximum;
    }

    void DoubleSliderWidget::set_value(double value)
    {
        this->value = value;
        slider.setValue((value - minimum) / single_step);
        spin_box.setValue(value);
    }

    double DoubleSliderWidget::get_value()
    {
        return value;
    }

    void DoubleSliderWidget::set_decimals(int decimals)
    {
        this->decimals = decimals;
        spin_box.setDecimals(decimals);
    }

    int DoubleSliderWidget::get_decimals()
    {
        return decimals;
    }

    void DoubleSliderWidget::notify_slider_value_changed(int value)
    {
        this->value = value * single_step + minimum ;

        spin_box.blockSignals(true);
        spin_box.setValue(this->value);
        spin_box.blockSignals(false);

        emit value_changed(this->value);
    }

    void DoubleSliderWidget::notify_spin_box_value_changed(double value)
    {
        this->value = value;

        slider.blockSignals(true);
        slider.setValue((value - minimum) / single_step);
        slider.blockSignals(false);

        emit value_changed(value);
    }

    void DoubleSliderWidget::update_range()
    {
        slider.setMinimum(0);
        slider.setMaximum(std::ceil((maximum - minimum) / single_step));

        spin_box.setMinimum(minimum);
        spin_box.setMaximum(maximum);

        set_value(value);
    }
}
