#include "StdAfx.h"
#include "ResourceStream.h"

CResourceStream::CResourceStream(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType):
	m_refCt(1),
	m_hRsrc(FindResource(hModule, lpName, lpType)),
	m_pData((const char*)LoadResource(hModule, m_hRsrc)),
	m_len(SizeofResource(hModule, m_hRsrc)),
	m_offset(0)
{
}

CResourceStream::~CResourceStream(void)
{
	if(m_hRsrc)
		FreeResource(m_hRsrc);
}
