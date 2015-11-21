/*
  ==============================================================================

	VersionChecker.cpp
	Created: 19 Aug 2015 1:21:14pm
	Author:  Parth

  ==============================================================================
*/
/* This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code by Parth.

MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.  */
#include "VersionChecker.h"

VersionChecker::VersionChecker() : Thread("VersionChecker")
{

}

void VersionChecker::run()
{
	URL versionURL("http://rsjaffe.github.io/MIDI2LR/version.xml");
	ScopedPointer<XmlElement> versionElem = versionURL.readEntireXmlStream();
	if (versionElem != nullptr && (versionElem->getIntAttribute("latest") > ProjectInfo::versionNumber))
	{
		_newVersion = versionElem->getIntAttribute("latest");
		triggerAsyncUpdate();
	}
}

void VersionChecker::handleAsyncUpdate()
{
	// show a dialog box indicating there is a newer version available
	DialogWindow::LaunchOptions dwOpt;
	dwOpt.dialogTitle = "New Version Available!";

	int major = (_newVersion & 0xF0000) >> 16;
	int minor = (_newVersion & 0x00F00) >> 8;
	int rev = (_newVersion & 0x0000F);
	String versionString = String::formatted("New version %d.%d.%d available", major, minor, rev);
	URL downloadURL("https://github.com/rsjaffe/MIDI2LR/releases/latest");

	dwOpt.content.setOwned(new HyperlinkButton(versionString, downloadURL));
	dwOpt.content->setSize(300, 100);
	((HyperlinkButton *)dwOpt.content.get())->setFont(Font(18.f), false);
	dwOpt.escapeKeyTriggersCloseButton = true;
	dwOpt.useNativeTitleBar = false;
	_dialog = dwOpt.create();
	_dialog->setVisible(true);
}
