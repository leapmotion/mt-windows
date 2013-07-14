#pragma once

DWORD DoInstallPackage(const wchar_t* infPath, BOOL& needReboot);

DWORD DoUninstallPackage(const wchar_t* infPath, BOOL& needReboot);