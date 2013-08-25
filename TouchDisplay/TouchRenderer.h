#pragma once
#include "DrawableItem.h"
#include <vector>

using std::vector;

class TouchRenderer:
  public vector<std::unique_ptr<DrawableItem>>
{
public:
  /// <summary>
  /// Adds a single mouse click
  /// </summary>
  void AddMouseClick(HWND hwnd, WPARAM wparam, LPARAM lparam);

  /// <summary>
  /// Parses and adds the specified touch inputs to this renderer
  /// </summary>
  void AddTouchInputs(HWND hwnd, WPARAM wparam, LPARAM lparam);

  /// <summary>
  /// Primary draw operation, draws all existing touches
  /// </summary>
  void Draw(HDC hdc) const;
};

