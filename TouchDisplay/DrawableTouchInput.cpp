#include "StdAfx.h"
#include "DrawableTouchInput.h"

extern HBRUSH g_hBrush[];
extern size_t g_hBrushCount;

DrawableTouchInput::DrawableTouchInput(HWND relTo, const TOUCHINPUT& input):
  m_dwID(input.dwID)
{
	m_pt.x = input.x / 100;
	m_pt.y = input.y / 100;
  m_ptEnd = m_pt;

	// Translate to screen coordinates
	ScreenToClient(relTo, &m_pt);
}

void DrawableTouchInput::Update(HWND relTo, const TOUCHINPUT& input) {
  m_ptEnd.x = input.x / 100;
  m_ptEnd.y = input.y / 100;

  ScreenToClient(relTo, &m_ptEnd);
}

void DrawableTouchInput::Draw(HDC hdc) const {
	RECT rc = {-2, -2, 2, 2};
	OffsetRect(&rc, m_pt.x, m_pt.y);
	InflateRect(&rc, 5, 5);
	FillRect(hdc, &rc, g_hBrush[m_dwID % g_hBrushCount]);

  if(m_pt.x != m_ptEnd.x || m_pt.y != m_ptEnd.y) {
    // Draw source to dest:
    Polyline(hdc, &m_pt, 2);

    // Draw the endpoint:
    Rectangle(hdc, m_ptEnd.x - 2, m_ptEnd.y - 2, m_ptEnd.x + 2, m_ptEnd.y + 2);
  }
}
