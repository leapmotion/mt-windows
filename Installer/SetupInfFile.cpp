#include "StdAfx.h"
#include "SetupInfFile.h"
#include <sstream>
#include <string>
#include <vector>

using namespace std;

SetupInfFile::SetupInfFile(const wchar_t* path):
  m_isLeap(false)
{
  UINT err;
  wchar_t buf[0x100];
  INFCONTEXT ctxt;

  m_hInf = SetupOpenInfFile(path, nullptr, INF_STYLE_WIN4, &err);
  if(m_hInf == INVALID_HANDLE_VALUE)
    throw eHidInstDriverInfOpenFailed;

  // Determine if this is a Leap driver:
  if(
    !SetupFindFirstLine(m_hInf, L"Version", L"Provider", &ctxt) ||
    !SetupGetStringField(&ctxt, 1, buf, sizeof(buf), nullptr) ||
    wcscmp(L"Leap Motion, Inc.", buf)
  )
    return;
  
  // Obtain version info:
  wstring version;
  if(
    !SetupFindFirstLine(m_hInf, L"Version", L"DriverVer", &ctxt) ||
    !SetupGetStringField(&ctxt, 1, buf, sizeof(buf), nullptr) ||
    ((m_date = buf), false) ||
    !SetupGetStringField(&ctxt, 2, buf, sizeof(buf), nullptr) ||
    ((version = buf), false)
  )
    return;

  // Parse version number:
  wstringstream dotted(version);

  int x, y, z, w;
  wchar_t dot;
  if(!(dotted >> x >> dot >> y >> dot >> z >> dot >> w))
    return;

  // Recompose version struct
  m_version.x = x;
  m_version.y = y;
  m_version.z = z;
  m_version.w = w;

  // Success:
  m_isLeap = true;
}

SetupInfFile::~SetupInfFile(void)
{
  if(m_hInf != INVALID_HANDLE_VALUE)
    SetupCloseInfFile(m_hInf);
}

void SetupInfFile::ReadDriverVer(void) {
}