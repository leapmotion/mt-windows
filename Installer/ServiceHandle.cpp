#include "stdafx.h"
#include "ServiceHandle.h"
#include "ServiceControlManager.h"

ServiceHandle::ServiceHandle(ServiceControlManager& scm, const wchar_t* name):
  m_hSrv(OpenServiceW(scm, name, GENERIC_ALL))
{
  if(!m_hSrv)
    throw eHidInstServiceOpenFailed;
}

ServiceHandle::~ServiceHandle(void)
{
  if(m_hSrv)
    CloseServiceHandle(m_hSrv);
}
