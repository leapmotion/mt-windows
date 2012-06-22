#include "stdafx.h"
#include "OcuIcon.h"
#include "OcuHidInstance.h"
#include "OcuImage.h"
#include "OcuIconWindowClass.h"

COcuIcon::COcuIcon(const COcuHidInstance* pParent):
	m_pParent(pParent),
	m_pImage(nullptr),
	m_fx(0.0),
	m_fy(0.0)
{
	// Create the window with the class registered as part of application
	// initialization.  This window will be used to draw overlay icons.
	m_hWnd = CreateWindowExW(
		WS_EX_LAYERED |			// We'd like to specify a permanent DC with this window that we can use with transparency effects
		WS_EX_TOPMOST |			// The overlay window should, of course, be always-on-top.
		WS_EX_NOACTIVATE |		// The window should never receive focus
		WS_EX_TRANSPARENT,		// Transparency ensures that mouse clicks go through the window.
		MAKEINTRESOURCE(COcuIconWindowClass::GetAtom()),
		L"",					// No window title
		WS_POPUP |				// The window will not have a title bar, either.
		WS_VISIBLE,				// The window should be initially visible.
		0, 0, 0, 0,				// Initial size and position:  All zeroes.
		nullptr,				// No parent window
		nullptr,				// No menu required
		nullptr,				// Default hinstance
		this					// Creation parameter is this
	);

	// The base offset is the distance between the location of the window and the position requested by the caller.
	// Initially, the base offset is set to zero, but eventually, the base offset is set as the negative of the
	// position of (0, 0) on the chosen display device in the virtual desktop coordinate space.
	m_baseOffset.x = 0;
	m_baseOffset.y = 0;

	// We want to receive messages from this window to our WindowProc.  Thus, we set our USERDATA to be
	// equal to the this pointer, so that the window class can route messages correctly.
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	// We'll be rendering on the main display every time.
	SetIconDisplayIndex(0, pParent);
}

COcuIcon::~COcuIcon(void)
{
	// Unregister the parent image prior to destruction
	SetImage(nullptr, false);

	if(m_hWnd)
		// Delete the window used to render this icon
		DestroyWindow(m_hWnd);
}

LRESULT COcuIcon::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DISPLAYCHANGE:
		// Something changed with monitor resolutions.  The extent of the display needs to be
		// correspondingly changed.
		SetIconDisplayIndex(m_iDisplayIndex, m_pParent);
		break;
	case WM_NCHITTEST:
		// Hit testing.  In order to allow for proper pass-through to underlying windows, this
		// window must not report a hit on any area of its extent.  Thus, we always return the
		// same value here.
		return HTTRANSPARENT;
	}

	// Don't know what's going on, let the default window procedure handle things.
	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

void COcuIcon::SetVisibility(bool bVisible)
{
	// Simple forwarding call to Win32 to show or hide this window based on caller request.
	ShowWindow(m_hWnd, bVisible ? SW_SHOW : SW_HIDE);
}

eHidStatus COcuIcon::SetIconDisplayIndex(int iDisplayIndex, const COcuHidInstance* pParent)
{
	// Currently, the only value for the display index that is supported is zero.
	// TODO:  Support other display indexes based on the number of monitors detected
	// on this system.
	if(iDisplayIndex)
		return eHidIntrIndexOutOfBounds;

	// Get the resolution of the main display:
	DISPLAY_DEVICE dev;
	dev.cb = sizeof(dev);
	EnumDisplayDevices(nullptr, iDisplayIndex, &dev, 0);

	// TODO: The more resilient EnumDisplayMonitors function should be used here
	// instead.  It can recover information about the virtual desktop that
	// EnumDisplaySettings cannot, and this may be used to offset base coordinates
	// for SetPosition.
	DEVMODE mode;
	EnumDisplaySettings(dev.DeviceName, ENUM_CURRENT_SETTINGS, &mode);
	m_displayExtent.cx = mode.dmPelsWidth;
	m_displayExtent.cy = mode.dmPelsHeight;

	// The new parent display device must be assigned.  Currently there is only one,
	// corresponding to the main display, but when there are more than one, this
	// device must be correctly matched to the display index.
	m_pParent = pParent;

	return eHidIntrSuccess;
}

void COcuIcon::SetPosition(double fx, double fy)
{
	// Recover the hotspot of the base image, if an image is assigned.  Otherwise, the
	// hot spot will be all zeroes.
	POINT hotspot = {0, 0};
	if(m_pImage)
		hotspot = m_pImage->GetHotspot();

	// Update our own floating-point coordinate.
	m_fx = fx;
	m_fy = fy;

	// The window position is the base offset plus the translation of the requested coordinate.
	SetWindowPos(
		m_hWnd,
		nullptr,
		m_baseOffset.y + (DWORD)(m_displayExtent.cx * fx) - hotspot.x,
		m_baseOffset.x + (DWORD)(m_displayExtent.cy * fy) - hotspot.y,
		0,
		0,
		SWP_NOSIZE | SWP_NOZORDER	// We don't want SetWindowPos to touch the size or Z-order.
	);
}

eHidStatus COcuIcon::SendReport(BYTE bContactID, bool TipSwitch, double cx, double cy) const
{
	// Trivial forwarding call.  The only thing added, here, is our own local relative
	// spatial coordinates.
	return m_pParent->SendReport(bContactID, TipSwitch, m_fx, m_fy, cx, cy);
}

void COcuIcon::SetImage(COcuImage* pImage, bool update)
{
	if(m_pImage)
		m_pImage->Register(this, false);
	m_pImage = pImage;
	if(update)
		Update();
}

eHidStatus COcuIcon::Update(void)
{
	// If the icon is currently null, abort.
	if(!m_pImage)
		return eHidIntrSuccess;

	// Some utility structures, corresponding to the origin and image extents
	POINT ptSrc = {0, 0};
	SIZE wndSize = {m_pImage->GetWidth(), m_pImage->GetHeight()};

	// The blend function is used to describe how to blend the image we
	// assign with the background.  Our technique is AC_SRC_OVER, which means
	// that the alpha channel of the source image is used to set transparency,
	// and that the source image has a premultiplied alpha channel.
	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = ~0;
	blend.AlphaFormat = AC_SRC_ALPHA;

	// Need a DC for the screen
	HDC hDC = GetDC(nullptr);

	BOOL rs = UpdateLayeredWindow(
		m_hWnd,					// The window to be updated.
		hDC,					// The DC for the screen where the window will be rendered
		nullptr,				// Default destination point
		&wndSize,				// Size is our own window size--won't default correctly
		m_pImage->GetDC(),		// Source DC is the image's source DC
		&ptSrc,					// Source point is the origin
		0,						// No color key
		&blend,					// Blend function specified above
		ULW_ALPHA				// Alpha channel is in the image
	);

	// Done with our DC
	ReleaseDC(nullptr, hDC);

	// Standard return value switch
	return
		rs ?
		eHidIntrSuccess :
		eHidIntrUpdateLayeredWindowFailed;
}