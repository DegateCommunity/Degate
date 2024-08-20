/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2021 Dorian Bachelot
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

#include "Core/Utils/Utils.h"
#include "GUI/Workspace/WorkspaceNotifier.h"

namespace degate
{
    void WorkspaceNotifier::define(WorkspaceTarget target,
                                   WorkspaceNotification notification,
                                   std::function<void(void)> updater)
    {
        targets[target][notification] = updater;
    }

    void WorkspaceNotifier::undefine(WorkspaceTarget target)
    {
        targets[target].clear();
    }

    void WorkspaceNotifier::notify(WorkspaceTarget target, WorkspaceNotification notification)
    {
        // If true, the application is quitting, hence just stop here
        if (qApp == nullptr)
            return;

        // Check if we are in the main thread (the only space to do UI stuff)
        if (qApp->thread() != QThread::currentThread())
        {
            // Force execution in main thread
            execute_in_main_thread([&] {
                // Call the notify() function with proper args
                notify(target, notification);
            });

            return;
        }

        // Just to be sure we cath a possible regression of the upper method (force running notify() in the main thread)
        assert(qApp->thread() == QThread::currentThread());

        // We then, in the main thread, just iterate over targets and notification and, finally, run the associated function
        auto wtarget = targets.find(target);
        if (wtarget != targets.end())
        {
            auto notifier = wtarget->second;
            auto wnotification = notifier.find(notification);
            if (wnotification != notifier.end())
                wnotification->second();
        }
    }
} // namespace degate
