#include <QApplication>
#include "XDebugManager.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
  {
  QApplication a(argc, argv);
  MainWindow w;
  XDebugManager m(false);
  w.show();
  
  return a.exec();
  }
