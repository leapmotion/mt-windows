#pragma once

class COcuHidInstance;
class COcuImage;

/// <summary>
/// Represents a single icon to be rendered as an overlay somewhere on screen.
/// </summary>
/// <remarks>
/// An icon is rendered as a layered window.  Layered windows have special operating system acceleration and can be
/// transparent.  The icon is renderd using a topmost window set to have a transparency flag set.  The operating
/// system even handles the task of passing mouse messages down to windows occluded by this icon.
/// </remarks>
class COcuIcon
{
public:
	/// <summary>
	/// Creates a new icon with the specified instance as the initial parent.
	/// </summary>
	COcuIcon(const COcuHidInstance* pParent);
	~COcuIcon(void);

private:
	// This is the parent emulator device.
	const COcuHidInstance* m_pParent;

	// The image currently in use with this icon
	COcuImage* m_pImage;

	// This is the window handle corresponding to this OcuSpec icon
	HWND m_hWnd;

	// These parameters are used to decide where, on screen, to render the icon
	int m_iDisplayIndex;
	POINT m_baseOffset;
	SIZE m_displayExtent;

	// The current relative position of this icon:
	double m_fx;
	double m_fy;

public:
	// Accessor methods:
	HWND GetHwnd(void) const {return m_hWnd;}

	/// <summary>
	/// Sets the display this icon should appear on.
	/// </summary>
	/// <param name="iDisplayIndex">The desired display index.  Currently, only a value of zero is supported</param>
	/// <param name="pParent">The parent HID emulator.  This must correspond to the requested display index</param>
	/// <remarks>
	/// Currently, because the underlying device driver does not support rendering on anything except the
	/// primary display, all values except 0 will generate an error.  Optimally, the only parameter that
	/// would be passed would be the parent HID instance.
	/// </remarks>
	eHidStatus SetIconDisplayIndex(int iDisplayIndex, const COcuHidInstance* pParent);
	
	/// <summary>
	/// Sets the visibility of this icon
	/// </summary>
	void SetVisibility(bool bVisible);

	/// <summary>
	/// Updates the location of this icon, in screen-relative coordinates
	/// </summary>
	/// <remarks>
	/// The position parameters must be bound in the range [0, 1).  The coordinate (0, 0) corresponds to the
	/// top-left corner of the screen, and (1, 1) is just beyond the bottom-right corner.
	/// </remarks>
	void SetPosition(double fx, double fy);

	/// <summary>
	/// Sends a report for this contact point at the contact point's current location
	/// </summary>
	eHidStatus SendReport(BYTE bContactID, bool TipSwitch, double cx = 0.0, double cy = 0.0) const;

	/// <summary>
	/// Updates the image used by this icon
	/// </summary>
	/// <param name="update">Set to true if the icon should be updated before the function returns</param>
	void SetImage(COcuImage* pImage, bool update = true);

	/// <summary>
	/// Notifies this OcuIcon that its parent image has been updated
	/// </summary>
	eHidStatus Update(void);
	
	/// <summary>
	/// Window procedure for this instance.  Should default everything and not be called directly
	/// </summary>
	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};