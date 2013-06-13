#include "XDebugLogger.h"
#include "QDataStream"
#include "QDebug"
#include "QtGui/QStandardItemModel"
#include "XVector"
#include "QtWidgets/QApplication"
#include "QThread"

uint qHash(const Eks::DebugLogger::ServerData::OpenEvent &ev)
  {
  return qHash(QPair<const void *, Eks::ThreadEventLogger::EventID>(ev.thread, ev.id));
  }

namespace Eks
{

bool operator==(
    const Eks::DebugLogger::ServerData::OpenEvent &a,
    const Eks::DebugLogger::ServerData::OpenEvent &b)
  {
  return a.id == b.id && a.thread && b.thread;
  }

QDataStream &operator<<(QDataStream &s, const DebugLogger::LogEntry &l)
  {
  return s << l.time << (xuint64)l.thread << l.level << l.entry;
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::LogEntry &l)
  {
  xuint64 thr;
  s >> l.time >> thr >> l.level >> l.entry;
  l.thread = (void*)thr;
  return s;
  }

QDataStream &operator<<(QDataStream &s, const ThreadEventLogger::EventItem &l)
  {
  return s << l.time << (const xuint8&)l.type << l.location << l.id;
  }

QDataStream &operator>>(QDataStream &s, ThreadEventLogger::EventItem &l)
  {
  s >> l.time >> (xuint8&)l.type >> l.location >> l.id;

  return s;
  }

QDataStream &operator<<(QDataStream &s, const EventLogger::LocationReference &l)
  {
  xAssert(l.codeLocation);
  return s << l.data << l.codeLocation->file() << l.codeLocation->function() << l.codeLocation->line();
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::LocationList &l)
  {
  QString file, function;
  xsize line;
  s >> l.data >> file >> function >> line;

  l.allocatedLocation.setFile(file);
  l.allocatedLocation.setFunction(function);
  l.allocatedLocation.setLine(line);

  return s;
  }

QDataStream &operator<<(QDataStream &s, const DebugLogger::EventList &l)
  {
  return s << (xuint64)l.thread << *l.events;
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::EventList &l)
  {
  xuint64 thr;
  s >> thr >> l.inPlaceEvents;
  l.events = &l.inPlaceEvents;
  l.thread = (void*)thr;
  return s;
  }

DebugLogger *g_logger;
QtMessageHandler g_oldHandler;
void handler(QtMsgType t, const QMessageLogContext &c, const QString &m)
  {
  static bool inHandler = false;
  if(g_oldHandler)
    {
    g_oldHandler(t, c, m);
    }

  if(inHandler)
    {
    return;
    }
  inHandler = true;

  DebugLogger::LogEntry e;
  e.level = t;
  e.entry = m;
  e.time = Time::now();
  e.thread = QThread::currentThread();

  xAssert(g_logger)
  g_logger->emitLogMessage(e);

  inHandler = false;
}

X_IMPLEMENT_DEBUG_INTERFACE(DebugLogger)

DebugLogger::DebugLogger(DebugManager *, bool client)
  {
  // we need an instance to flush on close.
  xAssert(QApplication::instance());

  static Reciever recv[] =
    {
    recieveFunction<LogEntry, DebugLogger, &DebugLogger::onLogMessage>(),
    recieveFunction<EventList, DebugLogger, &DebugLogger::onEventList>(),
    recieveFunction<LocationList, DebugLogger, &DebugLogger::onLocations>()
    };

  setRecievers(recv, X_ARRAY_COUNT(recv));

  if(client)
    {
    xAssert(!g_logger);
    g_logger = this;
    g_oldHandler = qInstallMessageHandler(handler);

#if X_EVENT_LOGGING_ENABLED
    Core::eventLogger()->setEventWatcher(this);
#endif
    }
  else
    {
    _server.create<ServerData>(Eks::Core::defaultAllocator());
    _server->model = createDataModel<QStandardItemModel>();

    _server->model->setHorizontalHeaderLabels(QStringList() << "Message");
    }

#if X_EVENT_LOGGING_ENABLED
  startTimer(25);
#endif
  }

DebugLogger::~DebugLogger()
  {
  Core::eventLogger()->syncCachedEvents();

  if(g_logger)
    {
    qInstallMessageHandler(g_oldHandler);

    g_logger = 0;
    g_oldHandler = 0;

#if X_EVENT_LOGGING_ENABLED
    Core::eventLogger()->setEventWatcher(0);
#endif
    }
  }

void DebugLogger::emitLogMessage(const LogEntry &e)
  {
  sendData(e);
  }

void DebugLogger::timerEvent(QTimerEvent*)
  {
#if X_EVENT_LOGGING_ENABLED
    Core::eventLogger()->syncCachedEvents();
#endif
  }

void DebugLogger::onEvents(const QThread *thread, const ThreadEventLogger::EventVector &events)
  {
  EventList l;
  l.thread = thread;
  l.events = &events;

  sendData(l);
  }

void DebugLogger::onLocations(const EventLogger::EventLocationVector &locations)
  {
  LocationList l = { &locations };
  sendData(l);
  }

void DebugLogger::onLogMessage(const LogEntry &e)
  {
  if(_server)
    {
    static const QString statuses[] =
      {
      "Debug",
      "Warning",
      "Critical",
      "Fatal",
      "System"
      };

    QStandardItem *parentItem = _server->model->invisibleRootItem();

    const QString &status = statuses[e.level];

    auto item = new QStandardItem(status + ":\n" + e.entry);

    const DebugLocationWithData *loc = nullptr;

    item->setData(QVariant::fromValue(loc), Location);
    item->setData(QVariant::fromValue(e.time), MomentTime);
    item->setData(QVariant((xuint64)e.thread), Thread);

    parentItem->appendRow(item);
    }
  }

void DebugLogger::onEventList(const EventList &list)
  {
  if (_server)
    {
    QStandardItem *parentItem = _server->model->invisibleRootItem();

    xForeach(const auto &evt, *list.events)
      {
      if(evt.type == ThreadEventLogger::EventType::Begin ||
         evt.type == ThreadEventLogger::EventType::Moment)
        {
        auto item = new QStandardItem();

        auto location = findLocation(evt.location);

        item->setData(QVariant::fromValue(location), Location);

        if(evt.type == ThreadEventLogger::EventType::Begin)
          {
          ServerData::OpenEvent e;
          e.thread = list.thread;
          e.id = evt.id;

          item->setData(QVariant::fromValue(evt.time), StartTime);
          _server->openEvents[e] = item;
          }
        else if(evt.type == ThreadEventLogger::EventType::Moment)
          {
          item->setData(QVariant::fromValue(evt.time), MomentTime);
          }

        item->setData(QVariant((xuint64)list.thread), Thread);
        QString display;
        if(!location)
          {
          display = "No Data";
          }
        else if(!location->data().isEmpty())
          {
          display = location->data();
          }
        else
          {
          display = location->function();
          }
        item->setData(QVariant::fromValue<QString>(display), Qt::DisplayRole);

        parentItem->appendRow(item);
        }
      else if(evt.type == ThreadEventLogger::EventType::End)
        {
        ServerData::OpenEvent e;
        e.thread = list.thread;
        e.id = evt.id;

        QStandardItem *item = _server->openEvents[e];
        xAssert(item);
        if(item)
          {
          item->setData(QVariant::fromValue(evt.time), EndTime);
          _server->openEvents.remove(e);
          }
        }
      }
    }
  }

void DebugLogger::onCodeLocations(const LocationList &e)
  {
  _locations.resize(xMax(_locations.size(), (xsize)e.id));

  auto &location = _locations[e.id];
  location.setFile(e.allocatedLocation.file());
  location.setLine(e.allocatedLocation.line());
  location.setFunction(e.allocatedLocation.function());
  location.setData(e.data);
  }

const DebugLogger::DebugLocationWithData *DebugLogger::findLocation(Eks::EventLocation::ID id)
  {
  if(id > _locations.size())
    {
    return nullptr;
    }

  return &_locations[id];
  }

}
