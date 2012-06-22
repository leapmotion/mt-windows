#include "StdAfx.h"
#include "OcuIconWindowClass.h"
#include "OcuIcon.h"

// This instance ensures that class registration occurs as one of the setup operations
// for the image we're a part of.  The class will remain available until global deinitializers
// are called.
COcuIconWindowClass COcuIconWindowClass::s_instance;

COcuIconWindowClass::COcuIconWindowClass(void)
{
	// Set up the class structure.

	m_wndClass.style =
		CS_NOCLOSE |		// No close button
		CS_SAVEBITS;		// Required to enable click-through transparency

	// Window procedure is defined at the end of this file
	m_wndClass.lpfnWndProc = WindowProc;

	// We need enough space to store a pointer to the COcuIcon that will drive the window
	m_wndClass.cbWndExtra = sizeof(COcuIcon*);

	// No class-specific data required
	m_wndClass.cbClsExtra = 0;

	// The remainder of these are alld efault values
	m_wndClass.hInstance = nullptr;
	m_wndClass.hIcon = nullptr;
	m_wndClass.hCursor = nullptr;
	m_wndClass.hbrBackground = nullptr;
	m_wndClass.lpszMenuName = nullptr;

	// The name of the class will be locally defined as OcuOverlay
	m_wndClass.lpszClassName = L"OcuOverlay";

	// Finally, conduct the registration.
	m_atom = RegisterClass(&m_wndClass);
}

COcuIconWindowClass::~COcuIconWindowClass(void)
{
	// Clean up the window class.
	UnregisterClass(L"OcuOverlay", nullptr);
}

LRESULT CALLBACK COcuIconWindowClass::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get a pointer to the COcuIcon instance.
	LONG_PTR val = GetWindowLongPtr(hwnd, GWLP_USERDATA);

	// If the pointer is non-null, control is passed to the icon itself.
	// Otherwise, we allow the default window procedure to handle this message.
	return
		val ?
		((COcuIcon*)val)->WindowProc(uMsg, wParam, lParam) :
		DefWindowProc(hwnd, uMsg, wParam, lParam);
}