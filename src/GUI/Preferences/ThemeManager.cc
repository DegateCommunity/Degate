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

#include "ThemeManager.h"
#include "PreferencesHandler.h"

#include <QFile>
#include <QTextStream>

namespace degate
{
    ThemeManager::ThemeManager()
    {
        QObject::connect(&PreferencesHandler::get_instance(), SIGNAL(icon_theme_changed()), this, SLOT(update_icon_theme()));
        QObject::connect(&PreferencesHandler::get_instance(), SIGNAL(theme_changed()), this, SLOT(update_theme()));

        update_theme();
    }

    ThemeManager::~ThemeManager()
    {
    }

    QString ThemeManager::get_icon_path(const QString& icon_name)
    {
        switch (PREFERENCES_HANDLER.get_preferences().icon_theme)
        {
        case LIGHT_ICON_THEME:
            return ":/light/" + icon_name;
        case DARK_ICON_THEME:
            return ":/dark/" + icon_name;
        default:
            return ":/light/" + icon_name;
        }
    }

    void ThemeManager::update_icon_theme()
    {
        emit icon_theme_changed();
    }

    void ThemeManager::update_theme()
    {
        // Save default (native) style name
        static QString default_style_name = qApp->style()->objectName();

        // Reset style sheet
        qApp->setStyleSheet("");

        // File
        QFile* file = nullptr;
        QTextStream* text_stream = nullptr;

        // Default icon theme
        QPalette theme_palette;
        switch (PREFERENCES_HANDLER.get_preferences().theme)
        {
            case NATIVE_THEME:

                qApp->setStyle(QStyleFactory::create(default_style_name));
                qApp->setPalette(qApp->style()->standardPalette());

                break;
            case LIGHT_THEME:

                qApp->setStyle(QStyleFactory::create("Fusion"));
                qApp->setPalette(qApp->style()->standardPalette());

                break;
            case DARK_THEME:

                qApp->setStyle(QStyleFactory::create("Fusion"));
                theme_palette.setColor(QPalette::Window, QColor(53,53,53));
                theme_palette.setColor(QPalette::WindowText, Qt::white);
                theme_palette.setColor(QPalette::Base, QColor(25,25,25));
                theme_palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
                theme_palette.setColor(QPalette::ToolTipBase, Qt::white);
                theme_palette.setColor(QPalette::ToolTipText, Qt::white);
                theme_palette.setColor(QPalette::Text, Qt::white);
                theme_palette.setColor(QPalette::Button, QColor(53,53,53));
                theme_palette.setColor(QPalette::ButtonText, Qt::white);
                theme_palette.setColor(QPalette::BrightText, Qt::red);
                theme_palette.setColor(QPalette::Link, QColor(42, 130, 218));
                theme_palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
                theme_palette.setColor(QPalette::HighlightedText, Qt::black);

                theme_palette.setColor(QPalette::Disabled, QPalette::Window, theme_palette.window().color().lighter());
                theme_palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(128, 128, 128));
                theme_palette.setColor(QPalette::Disabled, QPalette::Base, theme_palette.base().color().lighter());
                theme_palette.setColor(QPalette::Disabled, QPalette::AlternateBase, theme_palette.alternateBase().color().lighter());
                theme_palette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
                theme_palette.setColor(QPalette::Disabled, QPalette::Button, theme_palette.button().color().lighter());
                theme_palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128));
                theme_palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(53, 53, 53));
                theme_palette.setColor(QPalette::Disabled, QPalette::Link, theme_palette.link().color().lighter());
                theme_palette.setColor(QPalette::Disabled, QPalette::Highlight, theme_palette.highlight().color().lighter());
                theme_palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(128, 128, 128));

                qApp->setPalette(theme_palette);

                qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #191919; border: 1px solid white; } QToolBar { border: none; }");

                break;
            case BLUE_DARK_THEME:

                file = new QFile(":/themes/blue_dark.qss");
                if (!file->open(QFile::ReadOnly | QFile::Text))
                {
                    throw std::runtime_error("Can't open the theme file.");
                }

                text_stream = new QTextStream(file);
                qApp->setStyleSheet(text_stream->readAll());

                theme_palette.setColor(QPalette::Link, QColor(42, 130, 218));
                theme_palette.setColor(QPalette::LinkVisited, theme_palette.link().color().lighter());

                qApp->setPalette(theme_palette);

                file->close();
                delete text_stream;
                delete file;

                break;
            case STYLE_SHEET_THEME:

                file = new QFile(QString::fromStdString(STYLE_SHEET_PATH));
                if (!file->open(QFile::ReadOnly | QFile::Text))
                {
                    // Fallback
                    auto preferences = PREFERENCES_HANDLER.get_preferences();
                    preferences.theme = LIGHT_THEME;
                    preferences.icon_theme = DARK_ICON_THEME;
                    PREFERENCES_HANDLER.update(preferences);

                    return;
                }

                // Set style
                text_stream = new QTextStream(file);
                qApp->setStyleSheet(text_stream->readAll());

                // Clean
                file->close();
                delete text_stream;
                delete file;

                break;
            default:
                break;
        }
    }

    std::string theme_to_string(Theme theme)
    {
        switch (theme)
        {
        case NATIVE_THEME:
            return "native";
        case LIGHT_THEME:
            return "light";
        case DARK_THEME:
            return "dark";
        case BLUE_DARK_THEME:
            return "blue_dark";
        case STYLE_SHEET_THEME:
            return "style_sheet";
        default:
            return "native";
        }
    }

    Theme string_to_theme(const std::string& theme)
    {
        if (theme == "native")
            return NATIVE_THEME;
        else if (theme == "light")
            return LIGHT_THEME;
        else if (theme == "dark")
            return DARK_THEME;
        else if (theme == "blue_dark")
            return BLUE_DARK_THEME;
        else if (theme == "style_sheet")
            return STYLE_SHEET_THEME;
        else
            return NATIVE_THEME;
    }

    std::string icon_theme_to_string(IconTheme theme)
    {
        switch (theme)
        {
        case LIGHT_ICON_THEME:
            return "light";
        case DARK_ICON_THEME:
            return "dark";
        default:
            return "dark";
        }
    }

    IconTheme string_to_icon_theme(const std::string& theme)
    {
        if (theme == "light")
            return LIGHT_ICON_THEME;
        else if (theme == "dark")
            return DARK_ICON_THEME;
        else
            return DARK_ICON_THEME;
    }
}
