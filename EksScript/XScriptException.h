#ifndef XSCRIPTEXCEPTION_H
#define XSCRIPTEXCEPTION_H

#include "XScriptGlobal.h"
#include "QString"

namespace XScript
{
class Value;

EKSSCRIPT_EXPORT Value toss(const QString &msg);

struct MissingThisException
  {
protected:
  QString msg;
  template <typename T>
  void init()
  {
    msg = QString("XScriptConvert::from<T>() returned NULL. Throwing to avoid dereferencing a NULL pointer!");
  }
  MissingThisException() {}

public:
  QString const &Buffer() const { return msg; }
  };
}

#endif // XSCRIPTEXCEPTION_H
