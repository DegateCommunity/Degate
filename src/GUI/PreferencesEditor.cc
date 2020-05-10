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

#include "GUI/PreferencesEditor.h"

namespace degate
{
	PreferencesEditor::PreferencesEditor(QWidget* parent) : QDialog(parent)
	{
        //////////
        // List of pages
        //////////
        pages["Appearance"] = new AppearancePreferencesPage(this);


        //////////
        // Other
        //////////

        // Base settings
        setWindowTitle("Preferences");
	    setBaseSize(parent->size() * 0.5);

	    // Get preferences
        preferences = PREFERENCES_HANDLER.get_preferences();

        // Pages list setup
        pages_list.setViewMode(QListWidget::ListMode);
        pages_list.setSpacing(2);
        pages_list.setMaximumWidth(128);

        // Insert pages
        for(auto& e : pages)
        {
            pages_list.addItem(e.first);
            preferences_pages.addWidget(e.second);
        }

		// Save
		save_button.setText("Apply");
		QObject::connect(&save_button, SIGNAL(clicked()), this, SLOT(validate()));

		// Cancel
		cancel_button.setText("Close");
		QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(close()));

        pages_list.setCurrentItem(pages_list.item(0));

        // Pages layout
        preferences_pages_layout.addWidget(&preferences_pages);
        preferences_pages_layout.addStretch(1);

        // Content widget
        auto area_content = new QWidget;
        area_content->setLayout(&preferences_pages_layout);

        // Scroll area setup
        scroll_area.setWidget(area_content);
        scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll_area.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll_area.setWidgetResizable(true);

        // Content layout
        content_layout.addWidget(&pages_list);
        content_layout.addWidget(&scroll_area);

        // Button layout
        buttons_layout.addStretch(1);
        buttons_layout.addWidget(&save_button);
        buttons_layout.addWidget(&cancel_button);

        // Global layout
        layout.addLayout(&content_layout);
		layout.addLayout(&buttons_layout);

		setLayout(&layout);

        QObject::connect(&pages_list, &QListWidget::currentItemChanged, this, &PreferencesEditor::change_page);
	}

	PreferencesEditor::~PreferencesEditor()
	{
        for(auto& e : pages)
        {
            delete e.second;
        }
	}

	void PreferencesEditor::validate()
	{
	    // Apply changes for every page
        for(auto& e : pages)
        {
            e.second->apply(preferences);
        }

		PREFERENCES_HANDLER.update(preferences);

		PREFERENCES_HANDLER.save();
	}

    void PreferencesEditor::change_page(QListWidgetItem* current, QListWidgetItem* previous)
    {
        if (!current)
            current = previous;

        preferences_pages.setCurrentIndex(pages_list.row(current));
    }
}
