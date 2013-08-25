#pragma once
#include "DrawableItem.h"

class DrawableTouchInput:
  public DrawableItem
{
public:
	DrawableTouchInput(HWND relTo, const TOUCHINPUT& input);

private:
	POINT m_pt;
  POINT m_ptEnd;
	DWORD m_dwID;
  HBRUSH m_hBrush;

public:
  void Update(HWND relTo, const TOUCHINPUT& input);

  void Draw(HDC hdc) const override;
};

