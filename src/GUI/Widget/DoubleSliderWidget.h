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

#ifndef __DOUBLESLIDERWIDGET_H__
#define __DOUBLESLIDERWIDGET_H__

#include <QSlider>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

namespace degate
{
    /**
     * @class DoubleSliderWidget
     * @brief Define a double precision slider.
     * 
     * @see QSlider
     */
    class DoubleSliderWidget : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * Create a double precision slider.
         * 
         * @param parent : the parent of the widget.
         */
        explicit DoubleSliderWidget(QWidget* parent = nullptr);
        ~DoubleSliderWidget() override = default;

        /**
         * Set the single step of the slider (the step between values), inferior or equal 1.
         *
         * @param single_step : the single step of the slider (inferior or equal 1).
         */
        void set_single_step(double single_step);

        /**
         * Get the single step of the slider (the step between values), inferior or equal 1.
         *
         * @return Returns the precision value.
         */
        double get_single_step();

        /**
         * Set the minimum possible value for the slider.
         *
         * @param minimum : the minimum value.
         */
        void set_minimum(double minimum);

        /**
         * Get the minimum possible value for the slider.
         *
         * @return Returns the minimum value.
         */
        double get_minimum();

        /**
         * Set the maximum possible value for the slider.
         *
         * @param maximum : the maximum value.
         */
        void set_maximum(double maximum);

        /**
         * Get the maximum possible value for the slider.
         *
         * @return Returns the maximum value.
         */
        double get_maximum();

        /**
         * Set the value, it need to be between the minimum and the maximum values.
         *
         * @param value : the new value.
         */
        void set_value(double value);

        /**
         * Get the current value of the slider, it will be between the minimum and the maximum.
         *
         * @return Returns the current value of the slider.
         */
        double get_value();

        /**
         * Set the precision of the spin box (the number of decimals shown).
         * The default value will be calculated using the single_step value (but will not be optimal in certain
         * conditions)
         *
         * @param decimals : the number of decimals shown on the spin box.
         */
        void set_decimals(int decimals);

        /**
         * Get the precision of the spin box (the number of decimals shown).
         * The default value will be calculated using the single_step value (but will not be optimal in certain
         * conditions). This default value is different from the value returned here and is used when this value is -1.
         *
         * @return Returns the number of decimals shown on the spin box.
         */
        int get_decimals();

    signals:
        /**
         * Emitted when the value changed.
         *
         * @param value : the new value.
         */
        void value_changed(double value);

    public slots:
        /**
         * Called when the QSlider value changed.
         *
         * @param value : the new value.
         */
        void notify_slider_value_changed(int value);

        /**
         * Called when the QDoubleSpinBox value changed.
         *
         * @param value : the new value.
         */
        void notify_spin_box_value_changed(double value);

    protected:
        /**
         * Update the range of the slider and the spin box regarding min, max and step.
         */
        void update_range();

    private:
        QHBoxLayout    layout;
        QSlider        slider;
        QDoubleSpinBox spin_box;

        double value;

        double single_step = 0.1;
        double minimum     = 0;
        double maximum     = 1;
        int    decimals    = -1;

    };
}

#endif //__DOUBLESLIDERWIDGET_H__
