#pragma once

#include "targetver.h"

#include <stdio.h>
#include <windows.h>
#include <GdiPlus.h>
#include <Psapi.h>

extern HINSTANCE g_hInstance;
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);