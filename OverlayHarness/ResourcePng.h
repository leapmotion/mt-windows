#pragma once
#include "Gdiplus.h"

/// <summary>
/// Convenience class for loading a resource-stored PNG into an HBITMAP
/// </summary>
class CResourcePng:
	public Gdiplus::Bitmap
{
public:
	CResourcePng(HINSTANCE hInstance, LPCWSTR lpPngName);
	CResourcePng(HINSTANCE hInstance, int intResource);
	~CResourcePng(void);

private:

public:
	HDC ConstructDC(void);
};

