#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sddl.h>

using namespace std;

typedef NTSTATUS (*t_ZwUserSendTouchInput)(HWND hWnd, int rdx, TOUCHINPUT* touchData, DWORD touchDataSize);

auto hmod = LoadLibrary(L"user32");
auto ZwUserSendTouchInput = (t_ZwUserSendTouchInput)GetProcAddress(hmod, LPCSTR(1500));

void TestFireSingleTouchInput(void) {
  POINT pt;

  // Identify a touch window located at the user's cursor coordinates:
  GetCursorPos(&pt);
  HWND hWnd = WindowFromPhysicalPoint(pt);
  cout << "Window is " << hWnd << endl;

  // Verify the window is a touch window:
  ULONG flags;
  if(!IsTouchWindow(hWnd, &flags)) {
    cout << "Not a touch window" << endl;
    return;
  }

  cout << "Top window is " << hWnd << endl;

  for(size_t i = 0; i < 10; i++) {
    TOUCHINPUT data;
    data.x = 0x00017609 + i * 20 * 100;
    data.y = 0x0000b7e5 + i * 20 * 100;
    data.hSource = nullptr;
    data.dwID = i;
    data.dwFlags = TOUCHEVENTF_PRIMARY | TOUCHEVENTF_DOWN | TOUCHEVENTF_INRANGE;
    data.dwMask = 0x00000000;
    data.dwTime = 0x0090541b + i * 10;
    data.dwExtraInfo = 0x00000000;
    data.cyContact = 0x00000000;
    data.cxContact = 0x00000000;
    ZwUserSendTouchInput(hWnd, 1, &data, sizeof(data));
  }
}

int main(int argc, char* argv[]) {
  HWND hwnd = (HWND)0xC2AB6;
  ULONG flags;

  if(IsTouchWindow(hwnd, &flags))
    cout << "Is touch!";

  if(!ZwUserSendTouchInput)
    throw std::runtime_error("Cannot find touch input entrypoint");

  MessageBoxW(nullptr, L"Delaying while attachment", nullptr, 0);
  Sleep(1500);

  TestFireSingleTouchInput();

  cout << "Done." << endl;
  _getch();
	return 222;
}

