#pragma once

#include "targetver.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <newdev.h>
#include <Shlwapi.h>
#include "../newdev/newdev.h"
#include "PreprocFlags.h"
#include <devguid.h>
#include "InstallerCodes.h"

#define NUMBER_OF(x) (sizeof(x) / sizeof((x)[0]))

// The PNP ID that the HIDEmulator uses:
extern const wchar_t gc_pnpID[];
extern const DWORD gc_pnpIDLen;