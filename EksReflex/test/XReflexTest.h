#ifndef XREFLEXTEXT_H
#define XREFLEXTEXT_H

#include "QObject"
#include "XCore"

class EksReflexTest : public QObject
  {
  Q_OBJECT

public:
  EksReflexTest()
    {
    }

  ~EksReflexTest()
    {
    }

private Q_SLOTS:
  void functionWrapTest();
  void functionInvokeTest();
  void classWrapTest();

private:
  Eks::Core core;
  };

#endif // XREFLEXTEXT_H
