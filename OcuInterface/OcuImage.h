#pragma once
#include <memory>
#include <set>
#include <vector>

using std::set;
using std::vector;
using std::unique_ptr;

enum eHidStatus;
class COcuIcon;
class COcuHidInstance;

/// <summary>
/// Represents a single icon registered with the OcuInterface.  Do not construct this class directly;
/// rather, construct it by calling <see cref="COcuInterface::AddIcon"/>.
/// </summary>
class COcuImage
{
public:
	COcuImage(COcuHidInstance* pParent);
	~COcuImage(void);

private:
	COcuHidInstance* m_pParent;

	// The DC representing this image
	HDC m_hDC;

	// The hot spot point and image properties:
	POINT m_hotspot;
	SIZEL m_size;

	// All registered icons:
	set<COcuIcon*> m_icons;

	/// <summary>
	/// Registers (or unregisters) the passed icon with this image.
	/// </summary>
	void Register(COcuIcon* pIcon, bool bRegister);

public:
	// Accessor methods:
	HDC GetDC(void) const {return m_hDC;}
	POINT GetHotspot(void) const {return m_hotspot;}
	DWORD GetWidth(void) const {return m_size.cx;}
	DWORD GetHeight(void) const {return m_size.cy;}
	
	/// <summary>
	/// Sets the "hot spot" for this icon.  Initially, the hot spot is the geometric center of the icon.
	/// </summary>
	void SetHotspot(DWORD x, DWORD y);

	/// <summary>
	/// Constructs a DC from the passed image file
	/// </summary>
	eHidStatus SetImage(const wchar_t* pwcsFileName, const POINT* pHotspot = nullptr);

	/// <summary>
	/// Constructs a DC from the passed Gdi+ bitmap
	/// </summary>
	/// <param name="bitmap">The Gdi+ bitmap containing the desired image data</param>
	/// <param name="pHotspot">The hot spot for the image.  If left null, it will default to the geometric center.</param>
	eHidStatus SetImage(Gdiplus::Bitmap& src, const POINT* pHotspot = nullptr);

	/// <summary>
	/// Constructs a DC to be used with this image from the passed rectangular array of pixels
	/// </summary>
	/// <param name="width">The width of the passed image</param>
	/// <param name="height">The height of the passed image</param>
	/// <param name="pitch">The distance, in bytes, between successive rows of the image.  May potentially be negative.</param>
	/// <param name="pHotspot">The hot spot for the image.  If left null, it will default to the geometric center.</param>
	/// <param name="pScan0">A pointer to the first scan line in the source image</param>
	eHidStatus SetImage(LONG width, LONG height, int pitch, const void* pScan0, const POINT* pHotspot = nullptr, Gdiplus::PixelFormat PixelFormat = PixelFormat32bppARGB);

	/// <summary>
	/// Sets the DC used as a source for this image
	/// </summary>
	/// <param name="pHotspot">The hot spot for the image.  If left null, it will default to the geometric center.</param>
	/// <remarks>
	/// The passed DC must have been created with CreateCompatibleDC.  Once assigned, this class
	/// takes responsibility for the cleanup of the HDC and the HBITMAP attached to it
	/// </remarks>
	void SetDC(HDC hDc, const POINT* pHotspot = nullptr);

	/// <summary>
	/// Triggers a cascading update to all attached icons
	/// </summary>
	eHidStatus Update(void);

	// Friend declarations:
	friend class COcuIcon;
};

