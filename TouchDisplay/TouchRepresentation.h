#pragma once
#include <vector>

using std::vector;

class TouchRepresentation
{
public:
	TouchRepresentation(HWND relTo, const TOUCHINPUT& input);
	TouchRepresentation(const POINT& pt);

private:
	bool m_bMouseClick;
	POINT m_pt;
	DWORD m_dwID;

public:
	bool IsMouseClick(void) const {return m_bMouseClick;}
	int GetX(void) const {return m_pt.x;}
	int GetY(void) const {return m_pt.y;}
	DWORD GetID(void) const {return m_dwID;}

	static bool Translate(vector<TouchRepresentation>& out, HWND relTo, WPARAM wParam, LPARAM lParam);
};

