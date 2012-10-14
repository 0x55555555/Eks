#ifndef XDEBUGLOGGER_H
#define XDEBUGLOGGER_H

#include "XDebugInterface.h"

class EKSDEBUG_EXPORT XDebugLogger : public XDebugInterface
  {
  X_DEBUG_INTERFACE(XDebugLogger)

public:
  ~XDebugLogger();

  struct LogEntry
    {
    enum
      {
      DebugMessageType = 1
      };

    xuint32 level;
    QString entry;
    };

  void emitLogMessage(const LogEntry &e);

protected:
  void onLogMessage(const LogEntry &e);
  };

#endif // XDEBUGLOGGER_H
