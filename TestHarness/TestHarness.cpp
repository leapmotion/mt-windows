#include "stdafx.h"
#include "../OcuInterface/OcuInterface.h"
#include <conio.h>
#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;

#define MAX_CONTACTS 3

enum eMenuOption
{
	eMOFingerTap,
	eMOFingerTapSlow,
	eMOFingerTwoTap,
	eMOTryZoom,
	eMOTryZoomExit,
	eMOSimulateMouse,
	eMORingOfMultiTouch,
	eMORingOfTouch,
	eMOExit
};

eMenuOption PrintMenu()
{
	for(;;)
	{
		char* types[] =
		{
			 "250ms finger tap at (0.5, 0.5)",
			 "750ms finger tap at (0.5, 0.5)",
			 "250ms finger two-tap at (0.5, 0.5) and (0.5, 0.6)",
			 "Slow zoom at (0.5, 0.5)",
			 "Slow zoom at (0.5, 0.5) then exit",
			 "Simulate a mouse click at (0.5, 0.5)",
			 "Ring of multitouch operations centered on (0.5, 0.5)",
			 "Ring of touch operations centered on (0.5, 0.5)"
		};

		cout << "Choose from one of the following to simulate:" << endl;
		for(size_t i = 0; i < ARRAYSIZE(types); i++)
			cout << setw(2) << i << ") " << types[i] << endl;

		char c;
		cin >> c;

		if('0' <= c && c <= '9')
			return (eMenuOption)(eMOFingerTap + c - '0');

		switch(c)
		{
		case 'x':
			return eMOExit;
		}
	}
}

void PrintErrorText(eHidStatus rs)
{
	wchar_t* errmsg = nullptr;
	FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		GetModuleHandle(nullptr),
		rs,
		0,
		(LPWSTR)&errmsg,
		0,
		nullptr
	);

	cout << "Error, result was 0x" << setw(8) << setfill('0') << hex << rs << endl;
	wcout << L"Error text: " << (errmsg ? errmsg :  L"(null)") << endl;
	if(errmsg)
		LocalFree(errmsg);
}

int main(int argc, char* argv[])
{
	COcuHidInstance* ocuhid = nullptr;

	// Focus application window test:
	{
		CFocusAppInfo info;
		Sleep(1000);
		info.Update();
		wcout << info;
	}

	COcuInterface ocu(true);
	{
    // Eliminate anything that isn't an ocuHID interface:
    for(auto q = ocu.begin(); q != ocu.end();)
      if(FAILED((*q)->Initialize()))
        q = ocu.erase(q);
      else
        q++;

		cout << "Detected " << ocu.size() << " Leap Motion HID interfaces" << endl;
		if(!ocu.size())
		{
			_getch();
			return -1;
		}

    ocuhid = ocu[0];
	}

	auto type = PrintMenu();
	cout << "Waiting 1s before starting...";
	Sleep(1000);
	cout << "OK" << endl;

	for(;;)
	{
		eHidStatus rs;
		switch(type)
		{
		case eMOFingerTap:
		case eMOFingerTapSlow:
			rs = ocuhid->SendReport(0, true, 0.5, 0.5);
			if(SUCCEEDED(rs))
				cout << "Simulating touch click.  Down...";
			else
				break;
			Sleep(type == eMOFingerTapSlow ? 750 : 250);
			rs = ocuhid->SendReport(0, false, 0.5, 0.5);
			if(SUCCEEDED(rs))
				cout << "up" << endl;
			break;
		case eMOFingerTwoTap:
			rs = ocuhid->SendReport(0, true, 0.5, 0.5);
			if(!SUCCEEDED(rs))
				break;
			cout << "Simulating touch click.  Down, ";

			rs = ocuhid->SendReport(1, true, 0.6, 0.5);
			if(!SUCCEEDED(rs))
				break;
			cout << "Down..." << endl;

			Sleep(250);
			
			rs = ocuhid->SendReport(0, false, 0.5, 0.5);
			if(SUCCEEDED(rs))
				break;
			cout << "Up, ";

			rs = ocuhid->SendReport(1, false, 0.6, 0.5);
			if(SUCCEEDED(rs))
				break;
			cout << "Up" << endl;
			break;
		case eMOTryZoom:
		case eMOTryZoomExit:
			rs = ocuhid->Zoom(0.5, 0.5, 0.1, 2000.0);
			if(type == eMOTryZoomExit)
				return 0;
			break;
		case eMOSimulateMouse:
			rs = ocuhid->SendReportMouse(true, false, 0.5, 0.5);
			if(SUCCEEDED(rs))
				cout << "Simulating mouse click.  Down...";
			else
				break;
			Sleep(250);
			rs = ocuhid->SendReportMouse(false, false, 0.5, 0.5);
			if(SUCCEEDED(rs))
				cout << "up" << endl;
			break;
		case eMORingOfMultiTouch:
		case eMORingOfTouch:
			for(byte cid = 0; ;)
			{
				double x = 0.5 + 0.1 * sin(cid * M_PI * 2 / 7);
				double y = 0.5 + 0.1 * cos(cid * M_PI * 2 / 7);

				bool bDown = type == eMORingOfMultiTouch ? cid < MAX_CONTACTS : cid & 1;
				byte touch = type == eMORingOfMultiTouch ? cid % MAX_CONTACTS : 0;

				rs = ocuhid->SendReport(touch, bDown, x, y);
				if(SUCCEEDED(rs))
					cout << "Touch (" <<
					(int)(x * 100) << ", " <<
					(int)(y * 100) << ") " <<
					setw(2) << (int)touch << " " << (bDown ? "down" : "up") << endl;
				else
					break;

				Sleep(250);
				cid = (cid + 1) % (MAX_CONTACTS * 2);
			}
			break;
		case eMOExit:
			return 0;
		}
		Sleep(1000);

		if(!SUCCEEDED(rs))
			cout << "Failed to write to the HID, error was " << setw(8) << hex << rs << ", gle was " << GetLastError() << endl;
	}

	return 0;
}

