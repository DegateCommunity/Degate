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

#ifndef __PROGRESSDIALOG_H__
#define __PROGRESSDIALOG_H__

#include "Core/Utils/ProgressControl.h"

#include <mutex>
#include <thread>
#include <atomic>
#include <QProgressDialog>
#include <QTimer>

namespace degate
{
    /**
     * @class ProgressDialog
     * @brief Dialog to run a job in another thread and see a progress bar during the process.
     *
     * It can use a ProgressControl too to show time left and allow cancel.
     * If there is no ProgressControl the cancel button will be hiding and force the window to close will close the app.
     *
     * By default, if there is no ProgressControl or if progress equals 0, the progress bar will pulse.
     *
     * The dialog will close himself automatically when the job end, if there is no job set it will stay open until
     * close function called.
     *
     * If a progress control for a job is set, it needs to control the WHOLE job, and not a portion of it. Otherwise
     * the cancellation will just not work or not as expected (you can't stop a thread, the progress control handle
     * that).
     *
     * The job will start running when the exec function of the dialog is called.
     *
     * @see QDialog
     */
    class ProgressDialog : public QProgressDialog
    {
    Q_OBJECT

    public:
        /**
         * Create a new progress dialog.
         *
         * @param text : the default text to show.
         * @param progress_control : the progress control of the job (can be nullptr). It needs to control the whole
         * job.
         * @param parent : the parent widget of the dialog.
         */
        explicit ProgressDialog(const QString& text,
                                const ProgressControl_shptr& progress_control,
                                QWidget* parent = nullptr);
        ~ProgressDialog() override;

        /**
         * Set the job to run. It needs to be called before the exec function to have an effect.
         * The job will start running when the exec function of the dialog is called.
         *
         * @param job : the job to run.
         */
        void set_job(std::function<void()> job);

        /**
         * Know if the job was canceled (only relevant when a progress control is set).
         *
         * @return Returns true if the job was canceled, false otherwise.
         */
        bool was_canceled();

        /**
         * Know if the job is finished or not.
         *
         * @return Returns true if the job is finished, false otherwise.
         */
        bool is_finished();

    public slots:
        /**
         * Show the dialog and give him total control of the main thread.
         * It will also start the job in another thread if a job was set.
         *
         * @return Returns a status code.
         *
         * @see QDialog
         */
        int exec() override;

    protected slots:
        /**
         * Called by a timer periodically to update the progress and check if the job has finished.
         */
        void update_progress();

        /**
         * Called when a user tries to cancel the operation (close button or force the window to close).
         * If there is no ProgressControl the cancel button will be hiding and force the window to close will close the
         * app.
         */
        void cancel_operation();

    private:
        ProgressControl_shptr progress_control = nullptr;
        QTimer timer;
        std::function<void()> job;
        std::thread* thread = nullptr;
        std::mutex mutex;
        bool canceled = false;
        std::atomic<bool> finished;
    };
}

#endif //__PROGRESSDIALOG_H__
