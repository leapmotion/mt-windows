#include "StdAfx.h"
#include "TouchRenderer.h"
#include "DrawableItem.h"
#include "DrawableTouchInput.h"
#include "DrawableMouseClick.h"

void TouchRenderer::AddMouseClick(HWND hwnd, WPARAM wparam, LPARAM lparam) {
	POINT pt = {(LONG)(lparam & 0xFFFF), (LONG)(lparam >> 16)};
  
  // Perform minimal redraw:
  RECT rct = {pt.x, pt.y, pt.x, pt.y};
  InflateRect(&rct, 3, 3);
	InvalidateRect(hwnd, &rct, true);

  // Add the mouse click proper:
  emplace_back(
    std::unique_ptr<DrawableItem>(
      new DrawableMouseClick(pt)
    )
  );
}

void TouchRenderer::AddTouchInputs(HWND hwnd, WPARAM wparam, LPARAM lparam) {
	TOUCHINPUT rs[16];
	if(!GetTouchInputInfo((HTOUCHINPUT)lparam, ARRAYSIZE(rs), rs, sizeof(rs[0])))
		return;

  // Add each touch input present in this structure:
	for(size_t i = 0; i < (wparam & 0xFFFF); i++) {
    auto& cur = rs[i];

    DrawableTouchInput* ti;
    
    // New touch event?
    if(cur.dwFlags & TOUCHEVENTF_DOWN) {
      // New touch input, good to go:
      std::unique_ptr<DrawableTouchInput> item(new DrawableTouchInput(hwnd, rs[i]));
      ti = item.get();
      m_active[rs[i].dwID] = ti;
		  emplace_back(std::move(item));
    } else if(cur.dwFlags & TOUCHEVENTF_MOVE) {
      // Indicate that this point has moved
      m_active[rs[i].dwID]->Update(hwnd, cur);
    } else if(cur.dwFlags & TOUCHEVENTF_UP) {
      // Clear this point out
      m_active.erase(rs[i].dwID);
    }

    // Redraw this one:
	  InvalidateRect(hwnd, nullptr, true);
  }
}

void TouchRenderer::Draw(HDC hdc) const {
  for each(auto& touch in *this)
    touch->Draw(hdc);
}