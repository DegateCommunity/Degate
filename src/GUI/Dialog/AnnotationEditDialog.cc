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

#include "AnnotationEditDialog.h"

namespace degate
{
	AnnotationEditDialog::AnnotationEditDialog(Annotation_shptr annotation, QWidget* parent) : QDialog(parent), fill_color(parent), frame_color(parent), annotation(annotation)
	{
		text_label.setText(tr("Text:"));
		text.setText(QString::fromStdString(annotation->get_name()));
		
		fill_color_label.setText(tr("Fill color:"));
		fill_color.set_color(annotation->get_fill_color());
		
		frame_color_label.setText(tr("Frame color:"));
		frame_color.set_color(annotation->get_frame_color());

		validate_button.setText(tr("Ok"));
		cancel_button.setText(tr("Cancel"));

		layout.addWidget(&text_label, 0, 0);
		layout.addWidget(&text, 0, 1);
		layout.addWidget(&fill_color_label, 1, 0);
		layout.addWidget(&fill_color, 1, 1);
		layout.addWidget(&frame_color_label, 2, 0);
		layout.addWidget(&frame_color, 2, 1);
		layout.addWidget(&validate_button, 3, 0);
		layout.addWidget(&cancel_button, 3, 1);

		QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(validate()));
		QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

		setLayout(&layout);
	}

	AnnotationEditDialog::~AnnotationEditDialog()
	{
		
	}

	void AnnotationEditDialog::validate()
	{
		annotation->set_name(text.text().toStdString());
		annotation->set_fill_color(fill_color.get_color());
		annotation->set_frame_color(frame_color.get_color());

		accept();
	}
}
