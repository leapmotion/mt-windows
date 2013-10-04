#include "stdafx.h"
#include "../Installer/Installer.h"
#include <iostream>
#include <iomanip>
#include <conio.h>

using namespace std;

void PrintUsage(const wchar_t* argv[])
{
	wcout
    << "Usage: " << argv[0] << " [{install | update | uninstall | show} [infname]]" << endl
		<< endl
		<< " This application installs the Leap Motion HID legacy device.  It MUST be run in" << endl
		<< " administrator mode, because it has to create a new device.  By default, if no" << endl
		<< " option is specified, an installation will be attempted" << endl
		<< endl
		<< " The only difference between the install and update operations is that update" << endl
		<< " will fail if the driver is not currently installed." << endl;
}

int wmain(int argc, const wchar_t* argv[])
{
	eHidStatus rs;
	auto op = argc < 2 ? L"install" : argv[1];
	auto op2 = argc < 3 ? nullptr : argv[2];

	// Switch based on the requested operation
	if(!wcscmp(op, L"install"))
	{
		cout << "Installing, this could take a few minutes...";
		rs = OcuHidInstall(op2);
	}
	else if(!wcscmp(op, L"update"))
	{
		cout << "Updating, this could take a few minutes and may require a reboot...";
		rs = OcuHidUpdate(op2);
	}
	else if(!wcscmp(op, L"uninstall"))
	{
		cout << "Uninstalling, this could take a few minutes and may require a reboot...";
		rs = OcuHidUninstall(op2);
	}
  else if(!wcscmp(op, L"toggle"))
  {
    cout << "Installing, and then immediately uninstalling...";
    rs = OcuHidInstall(op2);
    if(SUCCEEDED(rs))
      rs = OcuHidUninstall(op2);
  }
	else
	{
		PrintUsage(argv);
		return -1;
	}

	// Show some error information based on the results of the install
	int gle = GetLastError();
	if(SUCCEEDED(rs))
		if(rs == eHidInstRestartRequired)
			cout << "Successful, but a reboot will be required" << endl;
		else
			cout << "Successful" << endl;
	else
	{
		wchar_t* errmsg = nullptr;

		// Installer.dll has error message strings compiled in as a resource section.  We
		// can use FormatMessage to get this information and format the return code correctly.
		FormatMessage(
			FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			GetModuleHandle(L"Installer.dll"),
			rs,
			0,
			(LPWSTR)&errmsg,
			0,
			nullptr
		);

		// Print the result literals:
		cout << "Error, result was 0x" << setw(8) << setfill('0') << hex << rs << endl;
		wcout << L"Error text: " << (errmsg ? errmsg :  L"(null)");
		if(errmsg)
			LocalFree(errmsg);

		if(gle)
		{
			// Format the system error code
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
				nullptr,
				gle,
				0,
				(LPWSTR)&errmsg,
				0,
				nullptr
			);
			cout << "GetLastError: " << gle << endl;
			wcout << L"System error text: " << (errmsg ? errmsg :  L"(null)");
			if(errmsg)
				LocalFree(errmsg);
		}
		else
			cout << "No GetLastError information" << endl;
	}

	return rs;
}

