#include "StdAfx.h"
#include "DrawableTouchInput.h"

extern HBRUSH g_hBrush[];
extern size_t g_hBrushCount;

DrawableTouchInput::DrawableTouchInput(HWND relTo, const TOUCHINPUT& input):
  m_dwID(input.dwID)
{
	m_pt.x = input.x / 100;
	m_pt.y = input.y / 100;

	// Translate to screen coordinates
	ScreenToClient(relTo, &m_pt);
}

void DrawableTouchInput::Draw(HDC hdc) const {
	RECT rc = {0, 0, 1, 1};
	InflateRect(&rc, 2, 2);
	OffsetRect(&rc, m_pt.x, m_pt.y);
	InflateRect(&rc, 5, 5);
	FillRect(
		hdc,
		&rc,
    g_hBrush[m_dwID % g_hBrushCount]
	);
}
