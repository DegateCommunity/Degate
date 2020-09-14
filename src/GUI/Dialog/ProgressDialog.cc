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

#include "ProgressDialog.h"

#include <utility>
#include <QCoreApplication>

namespace degate
{
    ProgressDialog::ProgressDialog(QWidget* parent, const QString& text, const ProgressControl_shptr& progress_control)
            : QProgressDialog(text, tr("Cancel"), 0, 0, parent),
              progress_control(progress_control),
              timer(this),
              finished(false)
    {
        timer.setInterval(100);

        // If no progress control disable the cancel button
        if (progress_control == nullptr)
            setCancelButton(nullptr);

        // Disable auto close when progress is at maximum
        setAutoClose(false);

        QObject::connect(this, &QProgressDialog::canceled, this, &ProgressDialog::cancel_operation);
        QObject::connect(&timer, &QTimer::timeout, this, &ProgressDialog::update_progress);
    }

    ProgressDialog::~ProgressDialog()
    {
        if (progress_control != nullptr)
            progress_control->cancel();

        delete thread;
    }

    void ProgressDialog::set_job(std::function<void()> job)
    {
        this->job = job;

        // Create the thread with the specified job and set the finished flag to true after it.
        thread = new std::thread([&, job]{ job(); finished = true; });
    }

    bool ProgressDialog::was_canceled()
    {
        std::lock_guard<std::mutex> lg(mutex);

        return canceled;
    }

    bool ProgressDialog::is_finished()
    {
        std::lock_guard<std::mutex> lg(mutex);

        return finished;
    }

    int ProgressDialog::exec()
    {
        if (job && thread != nullptr)
            thread->detach();

        timer.start(0);

        return QProgressDialog::exec();
    }

    void ProgressDialog::update_progress()
    {
        if (progress_control)
        {
            double progress = progress_control->get_progress() * 100;
            if (progress > 0)
            {
                setValue(static_cast<int>(progress));
                setMaximum(100);

                QString text;
                if (progress_control->has_log_message())
                    text = QString::fromStdString(progress_control->get_log_message());
                text += "\n";
                text += tr("Time left:") + " " + QString::fromStdString(progress_control->get_time_left_as_string());

                setLabelText(text);
            }
        }

        if (finished)
            accept();
    }

    void ProgressDialog::cancel_operation()
    {
        if (progress_control != nullptr)
            progress_control->cancel();
        else
            QCoreApplication::exit();

        canceled = true;
    }

}
