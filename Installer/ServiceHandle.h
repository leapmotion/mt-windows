#pragma once

class ServiceControlManager;

class ServiceHandle
{
public:
  ServiceHandle(ServiceControlManager& scm, const wchar_t* name);
  ~ServiceHandle(void);

private:
  SC_HANDLE m_hSrv;

public:
  operator SC_HANDLE(void) const {return m_hSrv;}
};

