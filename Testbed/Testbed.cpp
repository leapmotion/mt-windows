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
  // Identify an arbitrary touch window:
  HWND hWnd = nullptr;
  EnumWindows(
    [] (HWND hWnd, LPARAM lParam) -> BOOL {
      wchar_t text[0x20];
      GetWindowText(hWnd, text, 0x20);

      if(!IsTouchWindow(hWnd, 0))
        return true;

      *(HWND*)lParam = hWnd;
      return false;
    },
    (LPARAM)&hWnd
  );
  if(!hWnd)
    return;

  for(size_t i = 0; i < 10; i++) {
    TOUCHINPUT data;
    data.x = 0x00017609 + i * 20 * 100;
    data.y = 0x0000b7e5 + i * 20 * 100;
    data.hSource = nullptr;
    data.dwID = i;
    data.dwFlags = 0x0000001a;
    data.dwMask = 0x00000000;
    data.dwTime = 0x0090541b + i * 10;
    data.dwExtraInfo = 0x00000000;
    data.cyContact = 0x00000000;
    data.cxContact = 0x00000000;
    ZwUserSendTouchInput(hWnd, 1, &data, sizeof(data));
  }
}

int main(int argc, char* argv[]) {
  if(!ZwUserSendTouchInput)
    throw std::runtime_error("Cannot find touch input entrypoint");

  TestFireSingleTouchInput();
	return 222;
}

