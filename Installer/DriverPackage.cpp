#include <initguid.h>
#include <windef.h>
#include <difxapi.h>
#include "DriverPackage.h"

DWORD DoInstallPackage(const wchar_t* infPath, BOOL& needReboot) {
  // Use DifX to install drivers on the new devnode:
  return DriverPackageInstallW(
    infPath,
    DRIVER_PACKAGE_ONLY_IF_DEVICE_PRESENT,
    nullptr,
    &needReboot
  );
}

DWORD DoUninstallPackage(const wchar_t* infPath, BOOL& needReboot) {
  DWORD err = DriverPackageUninstallW(
    infPath,
    DRIVER_PACKAGE_DELETE_FILES,
    nullptr,
    &needReboot
  );
  
  switch(err) {
  case ERROR_DRIVER_PACKAGE_NOT_IN_STORE:
    // That's OK, it wasn't installed so it cannot be removed.  No problem.
    break;
  default:
    return err;
  }
  return 0;
}