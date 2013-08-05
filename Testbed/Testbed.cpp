#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sddl.h>

using namespace std;

struct TouchData {
  DWORD a1;
  DWORD a2;
  DWORD a3;
  DWORD a4;
  DWORD b1;
  DWORD b2;
  DWORD b3;
  DWORD b4;
  DWORD c1;
  DWORD c2;
  DWORD c3;
  DWORD c4;
};

typedef NTSTATUS (*t_ZwUserSendTouchInput)(HWND hWnd, int rdx, TouchData* touchData, DWORD touchDataSize);

auto hmod = LoadLibrary(L"user32");
auto procAddr = (t_ZwUserSendTouchInput)GetProcAddress(hmod, LPCSTR(1500));

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

  // Try to fire:
  TouchData data;
  data.a1 = 0x00017609;
  data.a2 = 0x0000b7e5;
  data.a3 = 0x004701a1;
  data.a4 = 0x00000000;
  data.b1 = 0x00000002;
  data.b2 = 0x0000001a;
  data.b3 = 0x00000000;
  data.b4 = 0x0090541b;
  data.c1 = 0x00000000;
  data.c2 = 0x00000000;
  data.c3 = 0x00000000;
  data.c4 = 0x00000000;

  cerr << procAddr(hWnd, 1, &data, sizeof(data)) << endl
       << GetLastError() << endl;
}

int main(int argc, char* argv[]) {
  if(!procAddr)
    throw std::runtime_error("Cannot find touch input entrypoint");

  TestFireSingleTouchInput();
	return 222;
}

