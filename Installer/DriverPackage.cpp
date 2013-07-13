#include <initguid.h>
#include <windef.h>
#include <difxapi.h>
#include "DriverPackage.h"

DWORD DoInstallPackage(const wchar_t* infPath, BOOL& needReboot) {
  // Use DifX to install drivers on the new devnode:
  return DriverPackageInstallW(
    infPath,
    0,
    nullptr,
    &needReboot
  );
}