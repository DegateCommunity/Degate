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

#ifndef __WORKSPACENOTIFIER_H__
#define __WORKSPACENOTIFIER_H__

#include "Core/Primitive/SingletonBase.h"

#include <QTimer>
#include <QThread>
#include <QCoreApplication>

#include <map>

namespace degate
{
    /**
     * @enum WorkspaceTarget
     * @brief Defines workspace targets that can be used with WorkspaceNotifier.
     */
    enum class WorkspaceTarget
    {
        WorkspaceBackground,
        Workspace
    };

    /**
     * @enum WorkspaceNotification
     * @brief Defines workspace notifications that can be used with WorkspaceNotifier.
     */
    enum class WorkspaceNotification
    {
        Update,
        Draw
    };

    /**
     * @class WorkspaceNotifier
     * @brief Singleton used to notify a workspace class (like for an update to perform).
     * 
     * @see WorkspaceBackground for example.
     */
    class WorkspaceNotifier : public SingletonBase<WorkspaceNotifier>
    {
        public:

            /**
             * Define a new notification for a target.
             * 
             * @param target : the target.
             * @param notification : the notification.
             * @param updater : the corresponding function.
             * 
             * @see WorkspaceTarget. 
             * @see WorkspaceNotification.
             */
            void define(WorkspaceTarget target, WorkspaceNotification notification, std::function<void(void)> updater);

            /**
             * Undefine all notifications for a target.
             * 
             * @param target : the target.
             * 
             * @see WorkspaceTarget. 
             */
            void undefine(WorkspaceTarget target);

            /**
             * Notify a target about something, like an update to perform (if defined).
             * 
             * @param target : the target to notify. 
             * @param notification : the notification.
             * 
             * @see WorkspaceTarget. 
             * @see WorkspaceNotification.
             */
            void notify(WorkspaceTarget target, WorkspaceNotification notification);

        private:
            std::map<WorkspaceTarget, std::map<WorkspaceNotification, std::function<void(void)>>> targets;
    };
}

#endif