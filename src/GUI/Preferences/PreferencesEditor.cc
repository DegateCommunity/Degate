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

#include "PreferencesEditor.h"

namespace degate
{
    PreferencesEditor::PreferencesEditor(QWidget* parent) : QDialog(parent)
    {
        // Create pages (go to the end of the file to add new ones).
        create_pages();

        // Base settings
        setWindowTitle(tr("Preferences"));
        //setBaseSize(parent->size() * 0.5);

        // Get preferences
        preferences = PREFERENCES_HANDLER.get_preferences();

        // Pages list setup
        pages_list.setViewMode(QListWidget::ListMode);
        pages_list.setSpacing(2);
        pages_list.setMaximumWidth(128);

        // Insert pages
        insert_pages();

        // Save
        save_button.setText(tr("Apply"));
        QObject::connect(&save_button, SIGNAL(clicked()), this, SLOT(validate()));

        // Cancel
        cancel_button.setText(tr("Close"));
        QObject::connect(&cancel_button, SIGNAL(clicked()), this, SLOT(close()));

        pages_list.setCurrentItem(pages_list.item(0));

        // Pages layout
        preferences_pages_layout.addWidget(&preferences_pages);

        // Content widget
        auto area_content = new QWidget;
        area_content->setLayout(&preferences_pages_layout);

        // Scroll area setup
        scroll_area.setWidget(area_content);
        scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
        QObject::connect(&PREFERENCES_HANDLER, &PreferencesHandler::language_changed, this, &PreferencesEditor::reload_editor);

        scroll_area.setMinimumWidth(area_content->minimumSizeHint().width());
    }

    PreferencesEditor::~PreferencesEditor()
    {
        for (auto& e : pages)
        {
            delete e.second;
        }
    }

    void PreferencesEditor::validate()
    {
        // Apply changes for every page
        for (auto& e : pages)
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

    void PreferencesEditor::reload_editor()
    {
        int current_index = pages_list.currentRow();

        save_button.setText(tr("Apply"));
        cancel_button.setText(tr("Close"));

        // Delete pages
        pages_list.clear();

        for (int i = preferences_pages.count(); i >= 0; i--)
        {
            QWidget* widget = preferences_pages.widget(i);
            preferences_pages.removeWidget(widget);
        }

        for (auto& e : pages)
            delete e.second;

        pages.clear();

        // Create pages
        create_pages();

        // Insert pages
        insert_pages();

        pages_list.setCurrentRow(current_index);
        preferences_pages.setCurrentIndex(current_index);

        adjustSize();
    }

    void PreferencesEditor::insert_page(const QString& name, PreferencesPage* page)
    {
        pages.push_back(std::pair<QString, PreferencesPage*>(name, page));
    }

    void PreferencesEditor::create_pages()
    {
        //////////
        // List of pages.
        //////////

        insert_page(tr("General"), new GeneralPreferencesPage(this));
        insert_page(tr("Appearance"), new AppearancePreferencesPage(this));
        insert_page(tr("Grid"), new GridPreferencesPage(this));
        insert_page(tr("Performances"), new PerformancesPreferencesPage(this));
    }

    void PreferencesEditor::insert_pages()
    {
        int index = 0;
        for (auto& e : pages)
        {
            pages_list.insertItem(index, e.first);
            preferences_pages.insertWidget(index, e.second);

            index++;
        }
    }
}
