#pragma once
#include <string>

#pragma pack(push, 1)
union INF_VERSION {
  struct {
    BYTE w;
    BYTE z;
    BYTE y;
    BYTE x;
  };
  DWORD version;
  
  bool operator<(const INF_VERSION& rhs) const {
    return version < rhs.version;
  }
};
#pragma pack(pop)

/// <summary>
/// Represents an open driver INF file
/// </summary>
class SetupInfFile
{
public:
  SetupInfFile(const wchar_t* path);
  ~SetupInfFile(void);

private:
  bool m_isLeap;
  HINF m_hInf;

  std::wstring m_date;
  INF_VERSION m_version;

public:
  bool IsLeap(void) const {return m_isLeap;}
  const INF_VERSION& GetVersion(void) const {return m_version;}

  /// <summary>
  /// Reads the "DriverVer" section out of the INF file
  /// </summary>
  void ReadDriverVer(void);
};


