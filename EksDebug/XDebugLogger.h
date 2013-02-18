#ifndef XDEBUGLOGGER_H
#define XDEBUGLOGGER_H

#include "XDebugInterface.h"

namespace Eks
{

class EKSDEBUG_EXPORT DebugLogger : public DebugInterface
  {
  X_DEBUG_INTERFACE(DebugLogger)

public:
  ~DebugLogger();

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

}

#endif // XDEBUGLOGGER_H
