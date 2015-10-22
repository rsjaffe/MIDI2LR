/*
  ==============================================================================

	VersionChecker.cpp
	Created: 19 Aug 2015 1:21:14pm
	Author:  Parth

  ==============================================================================
*/

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
