#include "XDebugLogger.h"
#include "QDataStream"
#include "QDebug"
#include "QtGui/QStandardItemModel"
#include "XVector"

namespace Eks
{

QDataStream &operator<<(QDataStream &s, const DebugLogger::LogEntry &l)
  {
  return s << l.level << l.entry;
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::LogEntry &l)
  {
  return s >> l.level >> l.entry;
  }

QDataStream &operator<<(QDataStream &s, const ThreadEventLogger::EventData &l)
  {
  if (l.hasEmbeddedLocation())
    {
    const CodeLocation &loc = l.location();
    s << loc.file() << loc.line() << loc.function();
    }
  else
    {
    auto &&loc = l.allocatedLocation();
    QByteArray data = loc.file.toUtf8();

    s << data.constData() << loc.line;

    data = loc.function.toUtf8();
    s << data.constData();
    }

  return s << l.data;
  }

QDataStream &operator>>(QDataStream &s, ThreadEventLogger::EventData &l)
  {
  ThreadEventLogger::EventData::Location loc;
  QByteArray arr;
  s >> arr >> loc.line;
  loc.file = QString::fromUtf8(arr);
  s >> arr;
  /*loc.function = QString::fromUtf8(arr);*/

  l.setAllocatedLocation(loc);
  s >> l.data;

  return s;
  }
QDataStream &operator<<(QDataStream &s, const ThreadEventLogger::EventItem &l)
  {
  return s << l.time << (const xuint8&)l.type << l.data << l.id;
  }

QDataStream &operator>>(QDataStream &s, ThreadEventLogger::EventItem &l)
  {
  s >> l.time >> (xuint8&)l.type >> l.data >> l.id;

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

  xAssert(g_logger)
  g_logger->emitLogMessage(e);

  inHandler = false;
}

X_IMPLEMENT_DEBUG_INTERFACE(DebugLogger)

DebugLogger::DebugLogger(DebugManager *, bool client)
  {
  static Reciever recv[] =
    {
    recieveFunction<LogEntry, DebugLogger, &DebugLogger::onLogMessage>(),
    recieveFunction<EventList, DebugLogger, &DebugLogger::onEventList>()
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
    _fb = createDataModel<QStandardItemModel>();

    _fb->setHorizontalHeaderLabels(QStringList() << "Message");
    }

#if X_EVENT_LOGGING_ENABLED
  startTimer(25);
#endif
  }

DebugLogger::~DebugLogger()
  {
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

void DebugLogger::onLogMessage(const LogEntry &e)
  {
  if(_fb)
    {
    static const QString statuses[] =
      {
      "Debug",
      "Warning",
      "Critical",
      "Fatal",
      "System"
      };

    QStandardItem *parentItem = _fb->invisibleRootItem();

    const QString &status = statuses[e.level];

    QList<QStandardItem*> items;
    items << new QStandardItem(status + ":\n" + e.entry);
    parentItem->appendRow(items);
    }
  }

void DebugLogger::onEventList(const EventList &e)
  {
  xForeach(const auto &evts, *e.events)
    {
    (void)evts;
    }
  }

}
