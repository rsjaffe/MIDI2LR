/*
  ==============================================================================

    KeySender.h
    Created: 1 Feb 2016 8:55:15pm
    Author:  Jeffrey

  ==============================================================================
*/

#ifndef KEYSENDER_H_INCLUDED
#define KEYSENDER_H_INCLUDED

// For windows documentation:
// https://blogs.msdn.microsoft.com/oldnewthing/20050530-11/?p=35513/


#ifdef _WIN32
// Because the SendInput function is only supported in
// Windows 2000 and later, WINVER needs to be set as
// follows so that SendInput gets defined when windows.h
// is included below.
#define WINVER 0x0500
#include <windows.h>
#endif

using namespace std;
#include <vector>

class KeySender
{
public:
	KeySender();
	virtual ~KeySender();
	void Configure(bool shift, bool alt, bool ctrl, char charToSend);
	void Execute(void);
private:
	bool m_shift;
	bool m_alt;
	bool m_ctrl;
	char m_charToSend;
	bool m_configured;
#ifdef _WIN32
	void AddSpecialKeys(vector< INPUT >&eventQueue, INPUT ip);
	DWORD FindProccessID(LPCTSTR ProcessName);
#endif
};




#endif  // KEYSENDER_H_INCLUDED
