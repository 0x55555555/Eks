#ifndef XSCRIPTSOURCE_H
#define XSCRIPTSOURCE_H

#include "XScriptGlobal.h"
#include "XProperty"
#include "QString"

class XScriptValue;
class QFile;

class EKSSCRIPT_EXPORT XScriptSource
  {
public:
  XScriptSource(QFile *file);
  XScriptSource(const QString &filename, const QString &data);

  class EKSSCRIPT_EXPORT Error
    {
  XProperties:
    XProperty(QString, trace, setTrace);
    XProperty(QString, message, setMessage);
    XProperty(bool, hasError, setHasError);
    XProperty(xuint32, lineNumber, setLineNumber);

  public:
    Error();
    };

  XScriptValue run(Error *error=0);

private:
  void load(const QString &key, const QString &src);
  void *_impl;
  };


#endif // XSCRIPTSOURCE_H
