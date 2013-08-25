#pragma once
#include "DrawableItem.h"

class DrawableMouseClick:
  public DrawableItem
{
public:
  DrawableMouseClick(POINT pt);

private:
	POINT m_pt;

public:
  virtual void Draw(HDC hdc) const override;
};

