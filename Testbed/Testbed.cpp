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

HANDLE token;
auto hmod = LoadLibrary(L"user32");
auto procAddr = (t_ZwUserSendTouchInput)GetProcAddress(hmod, LPCSTR(1500));
SERVICE_TABLE_ENTRY gc_table[] = {{L"Testbed", Run}, {nullptr, nullptr}};

// Impersonation token, used to turn off MIC:
HANDLE hImpersonation;

DWORD __stdcall HandlerProc(DWORD dwControl, DWORD dwEventType, void* lpEventData, void* lpContext) {
  return 0;
}

void TestFireSingleTouchInput(void) {
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

/// <summary>
/// Turns off mandatory policy on the current process token
/// </summary>
void DisableMIC(void) {
  __debugbreak();
  ImpersonateSelf(SecurityImpersonation);

  HANDLE hThreadToken;
  OpenThreadToken(GetCurrentThread(), MAXIMUM_ALLOWED, true, &hThreadToken);

  DWORD rs = 0;
  LUID relabel;
  LookupPrivilegeValue(nullptr, SE_RELABEL_NAME, &relabel);

  // Adjust privileges on ourselves first:
  TOKEN_PRIVILEGES tkp;
  tkp.PrivilegeCount = 1;
  tkp.Privileges[0].Luid = relabel;
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  // Grant ourselves the ability to modify our own MIC:
  AdjustTokenPrivileges(hThreadToken, false, &tkp, 0, nullptr, nullptr);

  // Duplicate our token:
  DuplicateTokenEx(hThreadToken, TOKEN_ALL_ACCESS, nullptr, SecurityImpersonation, TokenImpersonation, &hImpersonation);

  // Switch off mandatory policy:
  TOKEN_MANDATORY_POLICY policy;
  policy.Policy = 0;
  SetTokenInformation(hImpersonation, TokenMandatoryPolicy, &policy, sizeof(policy));

  // Flip over to our impersonation token:
  SetThreadToken(nullptr, hImpersonation);
}

void RunProxy(void) {
  DWORD rs;

  // Disable MIC first:
  DisableMIC();

  // Verify that our mandatory policy token is turned off:
  TOKEN_MANDATORY_POLICY policy;
  GetTokenInformation(token, TokenMandatoryPolicy, &policy, sizeof(policy), &rs);

  if(policy.Policy) {
    OutputDebugString(L"Mandatory policy flag was nonzero\n");
    return;
  }
}

void __stdcall Run(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors) {
  SERVICE_STATUS status;
  status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  status.dwWin32ExitCode = 0;
  status.dwServiceSpecificExitCode = 0;
  status.dwCheckPoint = 0;
  status.dwWaitHint = 0;

  // Okay, obtain a handle:
  auto hService = RegisterServiceCtrlHandlerExW(L"Testbed", HandlerProc, nullptr);
  
  // Running now:
  status.dwCurrentState = SERVICE_RUNNING;
  SetServiceStatus(hService, &status);

  RunProxy();

  status.dwCurrentState = SERVICE_STOPPED;
  SetServiceStatus(hService, &status);
}

int main(int argc, char* argv[]) {
  if(!procAddr)
    throw std::runtime_error("Cannot find touch input entrypoint");

  if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &token))
    throw std::runtime_error("Could not open process token");

  // Decide whether to run in service mode or interactive mode:
  DWORD sessionID;
  ProcessIdToSessionId(GetCurrentProcessId(), &sessionID);
  if(sessionID)
    RunProxy();
  else {
    OutputDebugString(L"Test test\n");
    StartServiceCtrlDispatcher(gc_table);
  }

  CloseHandle(token);
	return 0;
}

