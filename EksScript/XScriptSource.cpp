#include "XScriptSource.h"
#include "XScriptValue.h"
#include "XScriptFunction.h"
#include "XAssert"
#include "QFile"

namespace XScript
{

Source::Source(QFile *file)
  {
  _engineInterface = Engine::findInterface(file);
  xAssert(_engineInterface);

  EngineScope b(_engineInterface);
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
  xAssert(currentInterface() == engineInterface());
  Value ret;
  currentInterface()->runSource(&ret, this, error);

  return ret;
  }
}
