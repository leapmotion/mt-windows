#pragma once
#include "InstallerCodes.h"

/// <summary>
/// Installs the HidEmulator nondevice driver
/// </summary>
/// <param name="pInfPath">The path to the HidEmulator INF file</param>
/// <returns>A HID status value</returns>
extern "C" eHidStatus OcuHidInstall(const wchar_t* pInfPath);

/// <summary>
/// Updates the HidEmulator nondevice driver
/// </summary>
/// <param name="pInfPath">The path to the HidEmulator INF file</param>
/// <returns>A HID status value</returns>
extern "C" eHidStatus OcuHidUpdate(const wchar_t* pInfPath);

/// <summary>
/// Uninstalls the HidEmulator nondevice driver
/// </summary>
/// <param name="pInfPath">The path to the HidEmulator INF file</param>
/// <returns>A HID status value</returns>
extern "C" eHidStatus OcuHidUninstall(const wchar_t* pInfPath);
