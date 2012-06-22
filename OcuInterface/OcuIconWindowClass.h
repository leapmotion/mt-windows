#pragma once

/// <summary>
/// This is a utility class that registers the OcuIcon window class.  Do not use it directly.
/// </summary>
class COcuIconWindowClass
{
private:
	static COcuIconWindowClass s_instance;
	
	COcuIconWindowClass(void);
	~COcuIconWindowClass(void);

private:
	// The actual class definition:
	WNDCLASSW m_wndClass;

	// The window class atom:
	ATOM m_atom;

public:
	// The window class procedure:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	/// <returns>
	/// The class atom used to create windows of this class
	/// </returns>
	static ATOM GetAtom(void) {return s_instance.m_atom;}
};

