/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the FOO module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

function Component()
{
	component.loaded.connect(this, desktopShortcutCheckBox);
	component.loaded.connect(this, startMenuShortcutCheckBox);
    installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
    installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
}

desktopShortcutCheckBox = function()
{
	if (installer.value("os") === "win")
	{
		installer.addWizardPageItem( component, "DesktopShortcutCheckBoxForm", QInstaller.ReadyForInstallation );
	}
}

startMenuShortcutCheckBox = function()
{
	if (installer.value("os") === "win")
	{
		installer.addWizardPageItem( component, "StartMenuShortcutCheckBoxForm", QInstaller.StartMenuSelection );
	}
}

Component.prototype.createOperations = function()
{
    component.createOperations();
	
	try 
	{
		if (installer.value("os") === "win")
		{
			var isDesktopShortcutCheckBoxChecked = component.userInterface( "DesktopShortcutCheckBoxForm" ).desktopShortcutCheckBox.checked;
			if (isDesktopShortcutCheckBoxChecked) 
			{
				component.addOperation("CreateShortcut", "@TargetDir@/Degate.exe", "@DesktopDir@/Degate.lnk", "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/Degate.exe", "iconId=0", "description=Start Degate");
			}
			
			var isStartMenuShortcutCheckBoxChecked = component.userInterface( "StartMenuShortcutCheckBoxForm" ).startMenuShortcutCheckBox.checked;
			if (isStartMenuShortcutCheckBoxChecked) 
			{
				component.addOperation("CreateShortcut", "@TargetDir@/Degate.exe", "@StartMenuDir@/Degate.lnk", "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/Degate.exe", "iconId=0", "description=Start Degate");
			}
			
			if(systemInfo.currentCpuArchitecture.search("64") < 0) //x86
			{
				component.addElevatedOperation("Execute", "{0,1602,1638,3010}", "@TargetDir@/VC_redist.x86.exe", "/quiet", "/norestart");
			} 
			else //x64
			{
				component.addElevatedOperation("Execute", "{0,1602,1638,3010}", "@TargetDir@/VC_redist.x64.exe", "/quiet", "/norestart");
			}
		}
	} catch(e) 
	{
		console.log(e);
	}
}

Component.prototype.installationFinishedPageIsShown = function()
{
    try 
	{
        if (installer.isInstaller() && installer.status == QInstaller.Success) 
		{
            installer.addWizardPageItem( component, "ReadMeCheckBoxForm", QInstaller.InstallationFinished );
        }
    } catch(e) 
	{
        console.log(e);
    }
}

Component.prototype.installationFinished = function()
{
    try 
	{
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            var isReadMeCheckBoxChecked = component.userInterface( "ReadMeCheckBoxForm" ).readMeCheckBox.checked;
			
            if (isReadMeCheckBoxChecked) 
			{
                QDesktopServices.openUrl("https://github.com/DegateCommunity/Degate/blob/master/README.md");
            }
        }
    } catch(e) 
	{
        console.log(e);
    }
}

