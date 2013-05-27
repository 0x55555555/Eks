#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include "XDebugInterface.h"
#include "XDebugManager.h"
#include "mainwindow.h"
#include "logview.h"
#include "XCore"

class Watcher : public Eks::DebugManager::Watcher
  {
public:
  Watcher(MainWindow *w)
      : _main(w)
    {
    _log = 0;
    _logIfc = 0;
    }

  void onInterfaceRegistered(Eks::DebugInterface *ifc) X_OVERRIDE
    {
    if(ifc->typeName() == "DebugLogger")
      {
      _logIfc = ifc;
      auto log = new LogView(ifc->dataModel());
      _log = addDock(log);
      }
    }

  void onInterfaceUnregistered(Eks::DebugInterface *ifc) X_OVERRIDE
    {
    if(_logIfc == ifc)
      {
      delete _log;
      _log = 0;
      }
    }

  QDockWidget *addDock(QWidget *widg)
    {
    QDockWidget *dock = new QDockWidget(widg->objectName());
    dock->setWidget(widg);
    _main->addDockWidget(Qt::LeftDockWidgetArea, dock);
    return dock;
    }

  MainWindow *_main;

  QDockWidget *_log;
  Eks::DebugInterface *_logIfc;
  };

int main(int argc, char *argv[])
  {
  QApplication a(argc, argv);

  Eks::Core core;
  
  MainWindow w;

  Watcher watch(&w);
  Eks::DebugManager m(false, &watch);



  w.show();

  return a.exec();
  }
