/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2021 Dorian Bachelot

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

#include "CrashReport.h"

#include "Core/Version.h"
#include "Globals.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QStyle>
#include <fstream>

namespace degate
{
    CrashReport::CrashReport(const std::string& error) : QDialog(nullptr)
    {
        setWindowTitle(tr("Critical error"));
        setWindowIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical));

        QString message = tr("An unexpected error occurred.");

        // Error label
        error_label.setText(message);
        layout.addWidget(&error_label);

        // Error message
        error_edit.setTextColor(QColor(150, 10, 10));
        error_edit.setText(QString::fromStdString(error));
        error_edit.setReadOnly(true);
        layout.addWidget(&error_edit);

        // Open error file location button
        open_error_file_button.setText(tr("Open error file location"));
        QObject::connect(&open_error_file_button, &QPushButton::clicked, this, [=]() {
            QDesktopServices::openUrl(QUrl(QString::fromStdString(DEGATE_CONFIGURATION_PATH)));
        });
        buttons_layout.addWidget(&open_error_file_button);

        // Report button
        report_button.setText(tr("Report"));
        QObject::connect(&report_button, &QPushButton::clicked, this, [=]() {
            QDesktopServices::openUrl(QUrl("https://github.com/DegateCommunity/Degate/issues/new?template=bug_report.md"));
        });
        buttons_layout.addWidget(&report_button);

        // Quit button
        quit_button.setText(tr("Quit"));
        QObject::connect(&quit_button, &QPushButton::clicked, this, [=]() {
            this->close();
        });
        buttons_layout.addWidget(&quit_button);

        // Layout
        buttons_layout.addStretch(1);
        layout.addLayout(&buttons_layout);

        setLayout(&layout);
    }

    void crash_report(const std::string& error)
    {
        QString message = "[%1]\n\n"
                          "Build ABI: '%2'\n"
                          "Kernel type: '%3'\n"
                          "kernel version: '%4'\n"
                          "Current CPU architecture: '%5'\n"
                          "Build CPU architecture: '%6'\n"
                          "Product type: '%7'\n"
                          "Pretty product name: '%8'\n"
                          "Product version: '%9'\n"
                          "Degate version: '%10'\n"
                          "An unexpected error occurred. Error message:\n\n'%11'\n\n";

        message = message.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                          .arg(QSysInfo::buildAbi())
                          .arg(QSysInfo::kernelType())
                          .arg(QSysInfo::kernelVersion())
                          .arg(QSysInfo::currentCpuArchitecture())
                          .arg(QSysInfo::buildCpuArchitecture())
                          .arg(QSysInfo::productType())
                          .arg(QSysInfo::prettyProductName())
                          .arg(QSysInfo::productVersion())
                          .arg(DEGATE_VERSION)
                          .arg(QString::fromStdString(error));

        std::ofstream log_file;
        log_file.open(DEGATE_IN_CONFIGURATION(ERROR_FILE_NAME), std::fstream::in | std::fstream::out | std::fstream::app);

        if (!log_file.is_open())
            return;

        log_file << message.toStdString();
        log_file << "-----------------------------------------------------\n";
        log_file.close();

        degate::create_error_dialog(message.toStdString());
    }

    void create_error_dialog(const std::string& error)
    {
        QApplication::closeAllWindows();

        CrashReport crash_report(error);
        crash_report.exec();
    }
} // namespace degate
