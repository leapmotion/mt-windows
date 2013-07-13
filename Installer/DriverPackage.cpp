#include <initguid.h>
#include <windef.h>
#include <difxapi.h>

DWORD DoInstallPackage(const wchar_t* infPath, BOOL& needReboot) {
  INSTALLERINFO_W info;

  // Use DifX to install drivers on the new devnode:
  return DriverPackageInstallW(
    infPath,
    0,
    nullptr,
    &needReboot
  );
}