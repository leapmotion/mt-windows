#pragma once
class GdiplusInitializer
{
public:
	GdiplusInitializer(void);
	~GdiplusInitializer(void);

private:
	ULONG_PTR m_gdiplusToken;
};

