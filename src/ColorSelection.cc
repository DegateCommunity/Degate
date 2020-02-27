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

#include "ColorSelection.h"
#include "Image.h"

namespace degate
{
    /**
     * Convert Qt QColor to degate color_t.
     *
     * @param color : the Qt QColor to convert.
     * @return Return the corresponding degate color.
     */
    color_t to_degate_color(const QColor& color)
    {
        color_t res;

        res = MERGE_CHANNELS(qRed(color.rgb()), qGreen(color.rgb()), qBlue(color.rgb()), color.alpha());

        return res;
    }

    /**
     * Convert degate color_t to Qt QColor.
     *
     * @param color : the degate color to convert.
     * @return Return the corresponding QColor.
     */
    QColor to_qt_color(const color_t& color)
    {
        QColor res;

        res = qRgb(MASK_R(color), MASK_G(color), MASK_B(color));
        res.setAlpha(MASK_A(color));

        return res;
    }


    // Color picker dialog

    ColorPickerDialog::ColorPickerDialog(QWidget *parent, color_t color) : QDialog(parent), color_dialog(parent), color(color), button_box(QDialogButtonBox::Ok)
    {
        // Alpha label
        aplha_label.setText("Transparency :");

        // Alpha slider
        alpha_slider.setMaximum(255);
        alpha_slider.setMinimum(0);
        alpha_slider.setValue(MASK_A(color));
        alpha_slider.setOrientation(Qt::Orientation::Horizontal);
        QObject::connect(&alpha_slider, SIGNAL(valueChanged(int)), this, SLOT(on_slide_changed(int)));

        // Alpha layout
        alpha_layout.addWidget(&aplha_label);
        alpha_layout.addWidget(&alpha_slider);

        // Color dialog
        color_dialog.setCurrentColor(to_qt_color(color));
        color_dialog.setOptions(QColorDialog::NoButtons | QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
        QObject::connect(&color_dialog, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(on_color_changed(const QColor&)));

        // Button box
        QObject::connect(&button_box, SIGNAL(accepted()), this, SLOT(validate()));

        // Color preview
        QPalette palette;
        palette.setColor(QPalette::Background, to_qt_color(color));
        color_preview.setPalette(palette);
        color_preview.setMinimumSize(100, 100);
        color_preview.setAutoFillBackground(true);

        // Layout
        layout.addWidget(&color_dialog);
        layout.addLayout(&alpha_layout);
        layout.addWidget(&color_preview);
        layout.addWidget(&button_box);

        setLayout(&layout);
    }

    ColorPickerDialog::~ColorPickerDialog()
    {

    }

    void ColorPickerDialog::on_slide_changed(int value)
    {
        color = MERGE_CHANNELS(qRed(color_dialog.currentColor().rgb()), qGreen(color_dialog.currentColor().rgb()), qBlue(color_dialog.currentColor().rgb()), alpha_slider.value());

        color_dialog.setCurrentColor(to_qt_color(color));

        // Update preview color
        QPalette palette;
        palette.setColor(QPalette::Background, to_qt_color(color));
        color_preview.setPalette(palette);
    }

    void ColorPickerDialog::validate()
    {
        close();
    }

    void ColorPickerDialog::on_color_changed(const QColor& color)
    {
        if(color.alpha() != alpha_slider.value())
            alpha_slider.setValue(color.alpha());

        this->color = to_degate_color(color);

        // Update preview color
        QPalette palette;
        palette.setColor(QPalette::Background, color);
        color_preview.setPalette(palette);
    }

    color_t ColorPickerDialog::get_color()
    {
        return color;
    }


    // Color selection button

	ColorSelectionButton::ColorSelectionButton(QWidget* parent, const QString& text) : QPushButton(parent)
	{
		setText(text);
        setFlat(true);
        setAutoFillBackground(true);

		QObject::connect(this, SIGNAL(clicked()), this, SLOT(update_color()));
	}

	ColorSelectionButton::~ColorSelectionButton()
	{
	}

	void ColorSelectionButton::set_color(const color_t& color)
	{
		this->color = color;

		// Update color
        QPalette palette;
        palette.setColor(QPalette::Button, to_qt_color(color));
        setPalette(palette);
	}

	color_t ColorSelectionButton::get_color()
	{
		return color;
	}

	void ColorSelectionButton::update_color()
	{
        ColorPickerDialog color_picker(parentWidget(), color);
        color_picker.exec();

        color = color_picker.get_color();

        // Update color
        QPalette palette;
        palette.setColor(QPalette::Button, to_qt_color(color));
        setPalette(palette);
	}
}
