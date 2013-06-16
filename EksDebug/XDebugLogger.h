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

class DebugLoggerData;

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
    XProperty(xuint32, id, setId);
    XProperty(QString, data, setData);
    };

  struct LocationList
    {
    enum
      {
      DebugMessageType = 3
      };

    const EventLogger::EventLocationVector *embeddedLocations;
    Eks::Vector<DebugLocationWithData, 1024> allocatedLocations;
    };

  void emitLogMessage(const LogEntry &e);

  struct ServerData
    {
    struct OpenEvent
      {
      const void *thread;
      ThreadEventLogger::EventID id;
      };

    Eks::UnorderedMap <OpenEvent, xsize> openEvents;
    Eks::UniquePointer<DebugLoggerData> model;
    Eks::Vector<DebugLocationWithData, 1024> _locations;
    };
  const DebugLocationWithData *findLocation(const Eks::EventLocation::ID id);

protected:
  void onLogMessage(const LogEntry &e);
  void onEventList(const EventList &e);
  void onCodeLocations(const LocationList &e);

  void timerEvent(QTimerEvent* event) X_OVERRIDE;
  void onEvents(
      const QThread *thread,
      const ThreadEventLogger::EventVector &) X_OVERRIDE;
  void onLocations(const EventLogger::EventLocationVector &) X_OVERRIDE;

  friend bool operator==(const Eks::DebugLogger::ServerData::OpenEvent &a, const Eks::DebugLogger::ServerData::OpenEvent &b);

  Eks::UniquePointer<ServerData> _server;
  };

class EKSDEBUG_EXPORT DebugLoggerData : public QObject
  {
  Q_OBJECT

signals:
  void eventCreated(
      const Eks::Time &time,
      ThreadEventLogger::EventType type,
      xuint64 thread,
      const QString &display,
      const xsize durationId,
      const DebugLogger::DebugLocationWithData *location);

  void eventEndUpdated(const xsize id, const xsize thread, const Eks::Time &endTime);
  };

Q_DECLARE_METATYPE(const DebugLogger::DebugLocationWithData*)
}

#endif // XDEBUGLOGGER_H
