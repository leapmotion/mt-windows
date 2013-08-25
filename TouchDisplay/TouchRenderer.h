#pragma once
#include "DrawableItem.h"
#include <unordered_map>
#include <vector>

using std::vector;

class DrawableTouchInput;

class TouchRenderer:
  public vector<std::unique_ptr<DrawableItem>>
{
private:
  /// <summary>
  /// Map of active touch inputs
  std::unordered_map<int, DrawableTouchInput*> m_active;

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

