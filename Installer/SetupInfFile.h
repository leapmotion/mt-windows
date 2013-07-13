#pragma once

/// <summary>
/// Represents an open driver INF file
/// </summary>
class SetupInfFile
{
public:
  SetupInfFile(const wchar_t* path);
  ~SetupInfFile(void);

private:
  HINF m_hInf;

public:
  /// <summary>
  /// Reads the "DriverVer" section out of the INF file
  /// </summary>
  void ReadDriverVer(void);
};


