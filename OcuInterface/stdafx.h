#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <SetupAPI.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <GdiPlus.h>

extern "C" {
	#include "hidsdi.h"
}

#include "../Globals/Interface.h"
#include "OcuInterfaceCodes.h"
#include "PreprocFlags.h"

// These are extracted from hidclass.h:
#define IOCTL_HID_SET_OUTPUT_REPORT 0xb0195