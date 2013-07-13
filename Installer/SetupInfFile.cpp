#include "StdAfx.h"
#include "SetupInfFile.h"
#include <vector>
using std::vector;

SetupInfFile::SetupInfFile(const wchar_t* path)
{
  DWORD reqSize;
  if(!SetupGetInfInformationW(path, INFINFO_DEFAULT_SEARCH, nullptr, 0, &reqSize))
    throw eHidInstInfInformationUnavailable;

  vector<char> data(reqSize);
  auto& info = (SP_INF_INFORMATION&)data[0];
  if(!SetupGetInfInformationW(path, INFINFO_DEFAULT_SEARCH, &info, (DWORD)data.size(), &reqSize))
    throw eHidInstInfInformationUnavailable;

  UINT err;
  m_hInf = SetupOpenInfFile(path, nullptr, INF_STYLE_WIN4, &err);
  if(m_hInf == INVALID_HANDLE_VALUE)
    throw eHidInstDriverInfOpenFailed;
}

SetupInfFile::~SetupInfFile(void)
{
  if(m_hInf != INVALID_HANDLE_VALUE)
    SetupCloseInfFile(m_hInf);
}

void SetupInfFile::ReadDriverVer(void) {
}