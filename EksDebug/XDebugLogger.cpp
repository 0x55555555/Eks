#include "XDebugLogger.h"
#include "QDebug"

QDataStream &operator<<(QDataStream &s, const XDebugLogger::LogEntry &l)
  {
  return s << l.level << l.entry;
  }

QDataStream &operator>>(QDataStream &s, XDebugLogger::LogEntry &l)
  {
  return s >> l.level >> l.entry;
  }

XDebugLogger *g_logger;
void (*g_oldHandler)(QtMsgType, const char *);
void handler(QtMsgType t, const char *m)
  {
  static bool inHandler = false;
  if(g_oldHandler)
    {
    g_oldHandler(t, m);
    }

  if(inHandler)
    {
    return;
    }
  inHandler = true;

  XDebugLogger::LogEntry e;
  e.level = t;
  e.entry = m;

  xAssert(g_logger)
  g_logger->emitLogMessage(e);

  inHandler = false;
}

X_IMPLEMENT_DEBUG_INTERFACE(XDebugLogger)

XDebugLogger::XDebugLogger(bool client)
  {

  static Reciever recv[] =
    {
    recieveFunction<LogEntry, XDebugLogger, &XDebugLogger::onLogMessage>()
    };

  setRecievers(recv, X_ARRAY_COUNT(recv));

  if(client)
    {
    xAssert(!g_logger);
    g_logger = this;
    g_oldHandler = qInstallMsgHandler(handler);
    }
  }

XDebugLogger::~XDebugLogger()
  {
  if(g_logger)
    {
    qInstallMsgHandler(g_oldHandler);

    g_logger = 0;
    g_oldHandler = 0;
    }
  }

void XDebugLogger::emitLogMessage(const LogEntry &e)
  {
  sendData(e);
  }

void XDebugLogger::onLogMessage(const LogEntry &e)
  {
  qDebug() << e.level << e.entry;
  }
