#ifndef XDEBUGLOGGER_H
#define XDEBUGLOGGER_H

#include "QtCore/QObject"
#include "XDebugInterface.h"
#include "XUniquePointer"
#include "XEventLogger"
#include "XTime"

class QStandardItemModel;

namespace Eks
{

class EKSDEBUG_EXPORT DebugLogger
    : public QObject,
      public DebugInterface,
      private EventLogger::Watcher
  {
  Q_OBJECT

  X_DEBUG_INTERFACE(DebugLogger)

public:
  ~DebugLogger();

  struct LogEntry
    {
    enum
      {
      DebugMessageType = 1
      };

    Eks::Time time;
    xuint32 level;
    QString entry;
    };

  void emitLogMessage(const LogEntry &e);

protected:
  void onLogMessage(const LogEntry &e);

  void timerEvent(QTimerEvent* event) X_OVERRIDE;
  void onEvents(
      void *thread,
      const ThreadEventLogger::EventVector &) X_OVERRIDE;

  class Feedback;
  Eks::UniquePointer<QStandardItemModel> _fb;
  };

}

#endif // XDEBUGLOGGER_H
