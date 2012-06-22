#include "StdAfx.h"
#include "GdiplusInitializer.h"

using namespace Gdiplus;

GdiplusInitializer::GdiplusInitializer(void)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}


GdiplusInitializer::~GdiplusInitializer(void)
{
	GdiplusShutdown(m_gdiplusToken);
}
