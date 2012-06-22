#include "StdAfx.h"
#include "ResourcePng.h"
#include "ResourceStream.h"

using namespace Gdiplus;

CResourcePng::CResourcePng(HINSTANCE hInstance, LPCWSTR lpPngName):
	Gdiplus::Bitmap(&CResourceStream(hInstance, lpPngName, L"PNG"))
{
}

CResourcePng::CResourcePng(HINSTANCE hInstance, int intResource):
	Gdiplus::Bitmap(&CResourceStream(hInstance, MAKEINTRESOURCE(intResource), L"PNG"))
{
}

CResourcePng::~CResourcePng(void)
{
}

HDC CResourcePng::ConstructDC(void)
{
	// Create the objects to be returned:
	HDC retVal = CreateCompatibleDC(nullptr);

	// Attempt to transform the source bits to the destination space:
	BitmapData bitmapData;
	LockBits(&Rect(0, 0, GetWidth(), GetHeight()), ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

	// Construct an information header based on the locked bits:
	BITMAPINFO info;
	auto& hdr = info.bmiHeader;
	hdr.biSize = sizeof(hdr);

	switch(bitmapData.PixelFormat)
	{
	case PixelFormat32bppARGB:
		hdr.biWidth = bitmapData.Width;
		hdr.biHeight = bitmapData.Height;
		hdr.biPlanes = 1;
		hdr.biBitCount = 32;
		hdr.biCompression = BI_RGB;
		hdr.biSizeImage = 0;
		hdr.biXPelsPerMeter = 0;
		hdr.biYPelsPerMeter = 0;
		hdr.biClrUsed = 0;
		hdr.biClrImportant = 0;
		break;
	default:
		DeleteDC(retVal);
		UnlockBits(&bitmapData);
		return nullptr;
	}

	// Attempt to perform the copy operation
	RGBQUAD* pvBits;
	HBITMAP hBitmap = CreateDIBSection(
		retVal,
		&info,
		DIB_RGB_COLORS,
		(void**)&pvBits,
		nullptr,
		0
	);
	if(!hBitmap)
	{
		DeleteDC(retVal);
		return nullptr;
	}

	SelectObject(retVal, hBitmap);
	memcpy(
		pvBits,
		bitmapData.Scan0,
		bitmapData.Height * bitmapData.Stride
	);

	// Alpha channel must be premultiplied alpha in order to be blended correctly.
	// The premultiplication is done here.
	for(size_t i = bitmapData.Height * bitmapData.Width; i--;)
	{
		int alpha = pvBits[i].rgbReserved;
		pvBits[i].rgbRed = pvBits[i].rgbRed * alpha >> 8;
		pvBits[i].rgbGreen = pvBits[i].rgbGreen * alpha >> 8;
		pvBits[i].rgbBlue = pvBits[i].rgbBlue * alpha >> 8;
	}

	// Release memory regardless, we can't keep it here.
	UnlockBits(&bitmapData);
	return retVal;
}