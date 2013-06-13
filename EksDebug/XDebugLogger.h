#ifndef XDEBUGLOGGER_H
#define XDEBUGLOGGER_H

#include "QtCore/QObject"
#include "XDebugInterface.h"
#include "XUniquePointer"
#include "XEventLogger"
#include "XUnorderedMap"
#include "XTemporaryAllocator"
#include "XTime"

class QStandardItem;
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

  enum Role
    {
    MomentTime = Qt::UserRole,
    StartTime,
    EndTime,
    Thread,
    Location
    };

  struct LogEntry
    {
    enum
      {
      DebugMessageType = 1
      };

    Eks::Time time;
    const void *thread;
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

    const void *thread;
    const ThreadEventLogger::EventVector *events;

    Eks::TemporaryAllocator alloc;
    ThreadEventLogger::EventVector inPlaceEvents;
    };


  class DebugLocation
    {
  XProperties:
    XProperty(QString, function, setFunction);
    XProperty(xsize, line, setLine);
    XProperty(QString, file, setFile);
    };

  class DebugLocationWithData : public DebugLocation
    {
  XProperties:
    XProperty(QString, data, setData);
    };

  struct EventLocation
    {
    enum
      {
      DebugMessageType = 3
      };

    Eks::EventLocation::ID id;

    const Eks::CodeLocation *codeLocation;
    QString data;

    DebugLocation allocatedLocation;
    };

  void emitLogMessage(const LogEntry &e);

  struct ServerData
    {
    struct OpenEvent
      {
      const void *thread;
      ThreadEventLogger::EventID id;
      };

    Eks::UnorderedMap <OpenEvent, QStandardItem *> openEvents;
    Eks::UniquePointer<QStandardItemModel> model;
    };
  const DebugLocationWithData *findLocation(const Eks::EventLocation::ID id);

protected:
  void onLogMessage(const LogEntry &e);
  void onEventList(const EventList &e);
  void onCodeLocation(const EventLocation &e);

  void timerEvent(QTimerEvent* event) X_OVERRIDE;
  void onEvents(
      const QThread *thread,
      const ThreadEventLogger::EventVector &) X_OVERRIDE;
  Eks::EventLocation::ID onCreateLocation(const CodeLocation &l, const QString &data) X_OVERRIDE;

  friend bool operator==(const Eks::DebugLogger::ServerData::OpenEvent &a, const Eks::DebugLogger::ServerData::OpenEvent &b);

  Eks::UniquePointer<ServerData> _server;

  Eks::EventLocation::ID _lastID;
  Eks::Vector<DebugLocationWithData, 1024> _locations;
  };

Q_DECLARE_METATYPE(const DebugLogger::DebugLocationWithData*)
}

#endif // XDEBUGLOGGER_H
