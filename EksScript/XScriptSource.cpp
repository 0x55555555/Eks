#include "XScriptSource.h"
#include "XScriptValue.h"
#include "XScriptFunction.h"
#include "XConvertFromScript.h"
#include "XAssert"
#include "XConvertToScript.h"
#include "QFile"

namespace XScript
{

Source::Source(QFile *file)
  {
  _interface = Engine::findInterface(file);
  xAssert(_interface);

  EngineScope b(_interface);
  currentInterface()->loadSource(this, file->fileName(), QString::fromUtf8(file->readAll()));
  }

Source::Source(const QString &key, const QString &data)
  {
  currentInterface()->loadSource(this, key, data);
  }

SourceError::SourceError() : _hasError(false)
  {
  }

Value Source::run(SourceError *error)
  {
  EngineScope  b(_interface);

  Value ret;
  currentInterface()->runSource(&ret, this, error);

  return ret;
  }
}
