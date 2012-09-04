#ifndef XSCRIPTSOURCE_H
#define XSCRIPTSOURCE_H

#include "XScriptGlobal.h"
#include "XProperty"
#include "QString"

class QFile;

namespace XScript
{

class Value;

class EKSSCRIPT_EXPORT SourceError
  {
XProperties:
  XProperty(QString, trace, setTrace);
  XProperty(QString, message, setMessage);
  XProperty(bool, hasError, setHasError);
  XProperty(xuint32, lineNumber, setLineNumber);

public:
  SourceError();
  };

class EKSSCRIPT_EXPORT Source
  {
public:
  Source(QFile *file);
  Source(const QString &filename, const QString &data);


  Value run(SourceError *error=0);

private:
  void *_impl;
  };

}

#endif // XSCRIPTSOURCE_H
