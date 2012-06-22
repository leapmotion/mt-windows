#include "StdAfx.h"
#include "OcuImage.h"
#include "OcuIcon.h"
#include <memory>

using namespace std;
using namespace Gdiplus;

COcuImage::COcuImage(COcuHidInstance* pParent):
	m_pParent(pParent),
	m_hDC(nullptr)
{
	m_size.cx = 0;
	m_size.cy = 0;
	m_hotspot.x = 0;
	m_hotspot.y = 0;
}

COcuImage::~COcuImage(void)
{
	// A full copy of our local icon set is required, as the Register routine
	// is going to be called during our destructor loop:
	auto icons = m_icons;

	// Any icons that refer to us must have their current image set to null,
	// in order to prevent references to a deleted object.
	for each(auto& pIcon in icons)
	{
		pIcon->SetImage(nullptr);
		pIcon->Update();
	}

	// Set the DC to null.  This will release any resources assocaited with
	// the current DC.
	SetDC(nullptr);
}

void COcuImage::Register(COcuIcon* pIcon, bool bRegister)
{
	if(bRegister)
		// Registration is being requested, add the icon to the set of known icons.
		m_icons.insert(pIcon);
	else
		// Unregistering, remove the icon from the set of known icons.
		m_icons.erase(pIcon);
}

eHidStatus COcuImage::SetImage(const wchar_t* pwcsFileName, const POINT* pHotspot)
{
	Bitmap bmp(pwcsFileName);
	if(bmp.GetLastStatus() != Ok)
		return eHidIntrFailedToLoad;
	return SetImage(bmp, pHotspot);
}

eHidStatus COcuImage::SetImage(Gdiplus::Bitmap& src, const POINT* pHotspot)
{
	BitmapData bitmapData;

	// Lock bits in the passed source image:
	if(src.LockBits(&Rect(0, 0, src.GetWidth(), src.GetHeight()), ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) != Ok)
		return eHidIntrFailedToLock;

	// Set our image internally based on this array of pixels:
	eHidStatus rs = SetImage(bitmapData.Width, bitmapData.Height, bitmapData.Stride, bitmapData.Scan0, pHotspot, bitmapData.PixelFormat);

	// Done with processing, unlock and return the response value.
	src.UnlockBits(&bitmapData);
	return rs;
}

eHidStatus COcuImage::SetImage(LONG width, LONG height, int pitch, const void* pScan0, const POINT* pHotspot, PixelFormat PixelFormat)
{
	// Construct an information header based on the locked bits:
	BITMAPINFO info;
	auto& hdr = info.bmiHeader;
	hdr.biSize = sizeof(hdr);

	// Verify that this is a supported pixel format:
	switch(PixelFormat)
	{
	case PixelFormat32bppARGB:
		hdr.biWidth = width;
		hdr.biHeight = height;
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
		return eHidIntrPixelFormatNotRecognized;
	}

	// Create the objects to be returned:
	HDC hDC = CreateCompatibleDC(nullptr);

	// Attempt to perform the copy operation
	RGBQUAD* pvQuadDest;
	HBITMAP hBitmap = CreateDIBSection(
		hDC,
		&info,
		DIB_RGB_COLORS,
		(void**)&pvQuadDest,
		nullptr,
		0
	);
	if(!hBitmap)
	{
		DeleteDC(hDC);
		return eHidIntrCreateDibSectionFailed;
	}

	// Attach the constructed DC to the bitmap:
	SelectObject(hDC, hBitmap);

	// Set up the source pointers
	const RGBQUAD* pCurQuadSrc = (const RGBQUAD*)pScan0;
	const char*& pCurLineSrc = (const char*&)pCurQuadSrc;

	// Alpha channel must be premultiplied alpha in order to be blended correctly.
	// The premultiplication is done here.
	for(size_t y = height; y--;)
	{
		for(size_t x = width; x--;)
		{
			int alpha = pCurQuadSrc[x].rgbReserved;
			pvQuadDest[x].rgbReserved = alpha;
			pvQuadDest[x].rgbRed = pCurQuadSrc[x].rgbRed * alpha >> 8;
			pvQuadDest[x].rgbGreen = pCurQuadSrc[x].rgbGreen * alpha >> 8;
			pvQuadDest[x].rgbBlue = pCurQuadSrc[x].rgbBlue * alpha >> 8;
		}

		// Increment by width and by pitch:
		pvQuadDest += width;
		pCurLineSrc += pitch;
	}

	// Attach the DC:
	SetDC(hDC, pHotspot);

	// Done, return.
	return eHidIntrSuccess;
}

void COcuImage::SetDC(HDC hDC, const POINT* pHotspot)
{
	if(m_hDC)
	{
		// A DC currently exists.  We must release any resources associated with
		// this DC and then destroy it.

		// Get the bitmap first and delete it
		HBITMAP hBitmap = (HBITMAP)GetCurrentObject(m_hDC, OBJ_BITMAP);

		// Delete the DC
		DeleteDC(m_hDC);

		// Delete the unbounded bitmap
		DeleteObject(hBitmap);
	}

	// Get a handle to the bitmap:
	HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
	BITMAP bmpInfo;

	// Get extended information about the bitmap
	if(GetObject(hBitmap, sizeof(bmpInfo), &bmpInfo))
	{
		// Record the width and height of the bitmap:
		m_size.cx = bmpInfo.bmWidth;
		m_size.cy = bmpInfo.bmHeight;
	}
	else
		m_size.cx = m_size.cy = 0;
	
	// Update hot spot information:
	if(hDC)
		// User is assigning a non-null DC.
		if(pHotspot)
			// User is assigning a hotspot for this DC.  Copy it over.
			m_hotspot = *pHotspot;
		else
		{
			// User has not specified an explicit hotspot location.  The hotspot
			// will therefore default to the center of the image.
			m_hotspot.x = m_size.cx / 2;
			m_hotspot.y = m_size.cy / 2;
		}
	else
		// Everything is all zeroes
		m_hotspot.x = m_hotspot.y = m_size.cx = m_size.cy = 0;

	// Select in the new DC:
	m_hDC = hDC;
}

eHidStatus COcuImage::Update(void)
{
	// Cascade updates to each icon that uses this image
	for each(auto& pIcon in m_icons)
		pIcon->Update();

	return eHidIntrSuccess;
}
