#pragma once

class DrawableItem
{
public:
  virtual void Draw(HDC hdc) const = 0;
};

