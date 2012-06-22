#include "StdAfx.h"
#include "TouchRepresentation.h"

TouchRepresentation::TouchRepresentation(HWND relTo, const TOUCHINPUT& input):
	m_bMouseClick(false),
	m_dwID(input.dwID)
{
	m_pt.x = input.x / 100;
	m_pt.y = input.y / 100;

	// Translate to screen coordinates
	ScreenToClient(relTo, &m_pt);
}

TouchRepresentation::TouchRepresentation(const POINT& pt):
	m_bMouseClick(true),
	m_pt(pt),
	m_dwID(~0)
{
}

bool TouchRepresentation::Translate(vector<TouchRepresentation>& out, HWND relTo, WPARAM wParam, LPARAM lParam)
{
	TOUCHINPUT rs[16];
	if(!GetTouchInputInfo((HTOUCHINPUT)lParam, ARRAYSIZE(rs), rs, sizeof(rs[0])))
		return false;

	for(size_t i = 0; i < (wParam & 0xFFFF); i++)
		out.push_back(TouchRepresentation(relTo, rs[i]));
	return true;
}