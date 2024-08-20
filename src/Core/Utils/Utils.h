/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2024 Dorian Bachelot
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

#ifndef __CORE_UTILS_UTILS_H__
#define __CORE_UTILS_UTILS_H__

#include <qapplication.h>
#include <qthread.h>
#include <qtimer.h>

namespace degate
{
    /**
     * Execute a lambda in the main thread.
     */
    template<typename F>
    inline void execute_in_main_thread(F&& target)
    {
        // Check if we are in the main thread (the only space to do UI stuff)
        if (qApp->thread() != QThread::currentThread())
        {
            // We are here in any thead but the main thread

            // Create a single shot and instant timer that will run in the main thread
            QTimer* timer = new QTimer();
            timer->moveToThread(qApp->thread());
            timer->setSingleShot(true);

            // Connect the timeout of the timer to the notify() function
            QObject::connect(timer, &QTimer::timeout, [=]() {
                // Main thread
                target();

                // Delete this time when possible
                timer->deleteLater();
            });

            // Invoke the start method that will run the previous lambda
            // This will run in the main thread
            QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));

            return;
        }
        else
        {
            // Main thread
            target();
        }
    }
} // namespace degate

#endif
