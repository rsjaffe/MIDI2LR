/*
  ==============================================================================

    KeySender.cpp
    Created: 1 Feb 2016 8:55:15pm
    Author:  Jeffrey

  ==============================================================================
*/

#include "KeySender.h"
#include <TlHelp32.h>

KeySender::KeySender() : m_shift(false), m_alt(false),m_ctrl(false),m_charToSend(0), m_configured(false)
{

}

KeySender::~KeySender()
{

}

void KeySender::Configure(bool shift, bool alt, bool ctrl, char charToSend)
{
	m_shift = shift;
	m_alt = alt;
	m_ctrl = ctrl;
	m_charToSend = charToSend;
	m_configured = true;
}

#ifdef _WIN32

vector<HWND> g_HWND;
BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		//g_HWND = hwnd;
		g_HWND.push_back(hwnd);		
	}
	return TRUE;
}

#endif


void KeySender::Execute(void)
{
	if (m_configured)
	{
#ifdef _WIN32
		
		//Get the handle of the ligthroom window.

		DWORD handle = FindProccessID("lightroom.exe");
		g_HWND.clear();

		if (handle)
		{			
			
			EnumWindows(EnumWindowsProcMy, handle);
		}

		if (!g_HWND.empty())
		{			

			//SetWindowPos(g_HWND, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);  // it will bring window at the most front but makes it Always On Top.
			//SetWindowPos(g_HWND, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE); // just after above call, disable Always on Top.
				
			for (auto const& value : g_HWND)
			{
				//SetForegroundWindow(value);
				char txt[100];
				// get the window text for the current handle
				GetWindowText(value, txt, 100);

				// check for the correct text
				if (strstr(txt, "Lightroom") != NULL)
				{
					//set this window to the foreground
					SetForegroundWindow(value);					
				}
			}



			//Bring the lightroom window to the front.
			//if (SetForegroundWindow(g_HWND))
			{
				/*
				vector< INPUT > EventQueue;

				// Create a generic keyboard event structure
				INPUT ip;
				ip.type = INPUT_KEYBOARD;
				ip.ki.wScan = 0;
				ip.ki.time = 0;
				ip.ki.dwExtraInfo = 0;
				ip.ki.dwFlags = 0; // 0 for key press 

				//press the special keys
				AddSpecialKeys(EventQueue, ip);

				ip.ki.wVk = m_charToSend;
				EventQueue.push_back(ip);

				//set the release flag		
				ip.ki.dwFlags = KEYEVENTF_KEYUP;
				//byte first
				EventQueue.push_back(ip);

				//release the special keys
				AddSpecialKeys(EventQueue, ip);

				//send the codes
				SendInput(EventQueue.size(), &EventQueue[0], sizeof(INPUT));
				*/

				
				
				// Create a generic keyboard event structure
				INPUT ip;
				ip.type = INPUT_KEYBOARD;
				ip.ki.wScan = 0;
				ip.ki.time = 0;
				ip.ki.dwExtraInfo = 0;
				ip.ki.dwFlags = 0; // 0 for key press

				if (m_ctrl)
				{
					// Press the "Ctrl" key
					ip.ki.wVk = VK_CONTROL;
					ip.ki.dwFlags = 0; // 0 for key press
					SendInput(1, &ip, sizeof(INPUT));
				}

				// Press the "V" key
				ip.ki.wVk = m_charToSend;
				ip.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &ip, sizeof(INPUT));
							

				// Release the "V" key
				ip.ki.wVk = m_charToSend;
				ip.ki.dwFlags = KEYEVENTF_KEYUP;
				SendInput(1, &ip, sizeof(INPUT));

				if (m_ctrl)
				{
					// Release the "Ctrl" key
					ip.ki.dwFlags = KEYEVENTF_KEYUP;
					ip.ki.wVk = VK_CONTROL;					
					SendInput(1, &ip, sizeof(INPUT));
				}
			}
		}	
		

#else
	// sorry currently no support.
	// Send money for a mac. ;-)
#endif
	}
}

#ifdef _WIN32
void KeySender::AddSpecialKeys(vector< INPUT >&eventQueue, INPUT ip)
{

	if (m_ctrl == true)
	{
		// Press the "Ctrl" key
		ip.ki.wVk = VK_CONTROL;
		eventQueue.push_back(ip);
	}
	if (m_shift)
	{
		// Press the "shift" key
		ip.ki.wVk = VK_SHIFT;
		eventQueue.push_back(ip);
	}

	if (m_alt)
	{
		// Press the "Ctrl" key
		ip.ki.wVk = VK_MENU;
		eventQueue.push_back(ip);
	}

}

DWORD KeySender::FindProccessID(LPCTSTR ProcessName)
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) { // must call this first
		do {			
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap); // close handle on failure
	return 0;
}







#endif