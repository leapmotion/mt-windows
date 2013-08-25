#include "StdAfx.h"
#include "DrawableMouseClick.h"

DrawableMouseClick::DrawableMouseClick(POINT pt):
  m_pt(pt)
{
}

void DrawableMouseClick::Draw(HDC hdc) const {
	RECT rc = {0, 0, 1, 1};
	InflateRect(&rc, 1, 1);
	OffsetRect(&rc, m_pt.x, m_pt.y);
	FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
}