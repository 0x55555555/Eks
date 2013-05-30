#ifndef XDEBUGLOGGER_H
#define XDEBUGLOGGER_H

#include "QtCore/QObject"
#include "XDebugInterface.h"
#include "XUniquePointer"
#include "XEventLogger"
#include "XTemporaryAllocator"
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

  struct EventList
    {
    enum
      {
      DebugMessageType = 2
      };

    EventList()
        : alloc(Eks::Core::temporaryAllocator()),
          inPlaceEvents(&alloc)
      {
      }

    void *thread;
    const ThreadEventLogger::EventVector *events;

    Eks::TemporaryAllocator alloc;
    ThreadEventLogger::EventVector inPlaceEvents;
    };

  void emitLogMessage(const LogEntry &e);

protected:
  void onLogMessage(const LogEntry &e);
  void onEventList(const EventList &e);

  void timerEvent(QTimerEvent* event) X_OVERRIDE;
  void onEvents(
      const QThread *thread,
      const ThreadEventLogger::EventVector &) X_OVERRIDE;

  class Feedback;
  Eks::UniquePointer<QStandardItemModel> _fb;
  };

}

#endif // XDEBUGLOGGER_H
