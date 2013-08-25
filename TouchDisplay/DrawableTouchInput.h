#pragma once
#include "DrawableItem.h"

class DrawableTouchInput:
  public DrawableItem
{
public:
	DrawableTouchInput(HWND relTo, const TOUCHINPUT& input);

private:
	POINT m_pt;
	DWORD m_dwID;

public:
  void Draw(HDC hdc) const override;
};

