#pragma once
class RestartRequiredTracker
{
public:
  RestartRequiredTracker(void):
    m_restartRequired(false)
  {}

private:
  bool m_restartRequired;

protected:
  void RequireRestart(void) {
    m_restartRequired = true;
  }

public:
  // Accessor methods:
  bool IsRestartRequired(void) const {return m_restartRequired;}
};

