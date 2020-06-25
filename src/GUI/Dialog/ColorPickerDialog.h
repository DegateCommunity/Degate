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

#ifndef __COLORSELECTION_H__
#define __COLORSELECTION_H__

#include "Core/Utils/MemoryMap.h"

#include <QPushButton>
#include <QColorDialog>
#include <QColor>
#include <QtWidgets/qslider.h>
#include <QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>

namespace degate
{
    /**
     * @class ColorPickerDialog
     * @brief Run a dialog where the user can pick a RGBA color.
     *
     * @see QColorDialog
     */
    class ColorPickerDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Create a color picker dialog.
         *
         * @param parent : the parent of the dialog.
         * @param color : the initial color.
         */
        ColorPickerDialog(QWidget* parent, color_t color = 0);
        ~ColorPickerDialog();

        /**
         * Get the picked color (call it after the dialog closed).
         *
         * @return Returns the picked color.
         */
        color_t get_color();

    public slots:
        /**
         * Update the alpha slider with new value.
         *
         * @param value : the new alpha/slider value.
         */
        void on_slide_changed(int value);

        /**
         * Close the dialog.
         */
        void validate();

        /**
         * Change the color.
         *
         * @param color : the new color.
         */
        void on_color_changed(const QColor& color);

    private:
        QVBoxLayout layout;
        QDialogButtonBox button_box;

        // Alpha
        QSlider alpha_slider;
        QLabel aplha_label;
        QHBoxLayout alpha_layout;

        // Color
        color_t color;
        QColorDialog color_dialog;
        QWidget color_preview;

    };

    /**
     * @class ColorSelectionButton
     * @brief A colored button that allow color selection on click.
     */
	class ColorSelectionButton : public QPushButton
	{
	    Q_OBJECT

	public:
	    /**
	     * Create a new color selection button.
	     *
	     * @param parent : the widget parent.
	     * @param text : the button text (default none).
	     */
		ColorSelectionButton(QWidget* parent = nullptr, const QString& text = QString());
		~ColorSelectionButton();

		/**
		 * Set the button color.
		 *
		 * @param color : the new color.
		 */
	    void set_color(const color_t& color);

	    /**
	     * Get the selected color.
	     *
	     * @return Returns the selected color.
	     */
	    color_t get_color();

	public slots:
	    /**
	     * Update the color button with current selected color.
	     */
	    void update_color();

	private:
	    color_t color;
	};
}

#endif