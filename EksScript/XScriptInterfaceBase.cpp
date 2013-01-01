#include "XScriptInterfaceBase.h"
#include "XUnorderedMap"
#include "XScriptEngine.h"

namespace XScript
{
InterfaceBase::InterfaceBase(xsize typeID,
               xsize nonPointerTypeID,
               const Eks::String &typeName,
               const InterfaceBase *parent)
  : _typeName(typeName),
    _typeId(typeID),
    _baseTypeId(parent->_baseTypeId),
    _nonPointerTypeId(nonPointerTypeID),
    _baseNonPointerTypeId(parent->_baseNonPointerTypeId),
    _toScript(parent->_toScript),
    _fromScript(parent->_fromScript),
    _parent(parent),
    _isSealed(true)
  {
  xAssert(_typeName.length());

  xAssertFail(); // should this be explicity
  buildInterface(0, 0, 0, 0, 0, 0);
  }

InterfaceBase::InterfaceBase(int typeId,
                               int nonPointerTypeId,
                               int baseTypeId,
                               int baseNonPointerTypeId,
                               const Eks::String &typeName,
                               const InterfaceBase *parent,
                               ToScriptFn tScr,
                               FromScriptFn fScr)
  : _typeName(typeName),
    _typeId(typeId),
    _baseTypeId(baseTypeId),
    _nonPointerTypeId(nonPointerTypeId),
    _baseNonPointerTypeId(baseNonPointerTypeId),
    _toScript(tScr),
    _fromScript(fScr),
    _parent(parent),
    _constructorCount(0),
    _propertyCount(0),
    _functionCount(0),
    _isSealed(false),
    _constructors(0),
    _properties(0),
    _functions(0)
  {
  xAssert(_typeName.length());
  }

InterfaceBase::~InterfaceBase()
  {
  xForeach(EngineInterface *ifc, Engine::interfaces())
    {
    ifc->removeInterface(this);
    }
  }

QVariant InterfaceBase::toVariant(const Value &inp, int typeHint)
  {
  if(_fromScript)
    {
    const void *val = _fromScript(inp);

    if(typeHint == _typeId || typeHint == 0)
      {
      return QVariant(_typeId, &val);
      }
    else if(typeHint == _nonPointerTypeId)
      {
      return QVariant(_nonPointerTypeId, val);
      }

    UpCastFn fn = _upcasts.value(typeHint, 0);
    if(fn)
      {
      void *typedPtr = fn((void*)val);
      return QVariant(typeHint, &typedPtr);
      }
    }

  return QVariant();
  }

Value InterfaceBase::copyValue(const QVariant &v) const
  {
  int type = v.userType();
  // T Ptr (to either a pointer or the actual type)
  const void **val = (const void**)v.constData();
  xAssert(val);

  if(_toScript)
    {
    if(type == _typeId)
      {
      // resolve to T Ptr
      return _toScript(*val);
      }
    else if(type == _nonPointerTypeId)
      {
      // already a pointer to T
      return _toScript(val);
      }
    }

  return Value();
  }

void InterfaceBase::seal()
  {
  _isSealed = true;
  }

void InterfaceBase::wrapInstance(Object *scObj, void *object) const
  {
  currentInterface()->wrapInstance(this, scObj, object);
  }

void InterfaceBase::unwrapInstance(Object *scObj) const
  {
  currentInterface()->unwrapInstance(this, scObj);
  }

Object InterfaceBase::newInstance(int argc, Value argv[], const QString& name) const
  {
  Object obj;
  currentInterface()->newInstance(&obj, this, argc, argv, name);
  return obj;
  }

void InterfaceBase::buildInterface(
    const ConstructorDef *ctors,
    xsize ctorCount,
    const PropertyDef *props,
    xsize propCount,
    const FunctionDef *fns,
    xsize fnCount)
  {
  xAssert(!isSealed());

  _constructors = ctors;
  _constructorCount = ctorCount;

  _properties = props;
  _propertyCount = propCount;

  _functions = fns;
  _functionCount = fnCount;

  seal();
  }

/*void InterfaceBase::set(const char *name, Value val)
  {
#ifdef X_DART
  xAssertFail();
#else
  (*::prototype(_prototype))->Set(v8::String::NewSymbol(name), getV8Internal(val));
#endif
  }*/

#if 0
void InterfaceBase::addConstructor(
    const char *cname,
    xsize extraArgs,
    size_t argCount,
    FunctionJS fn,
    FunctionDart ctor)
  {
#ifdef X_DART
  QString callArgs;
  for(size_t i = 0; i < argCount; ++i)
    {
    callArgs += "_" + QString::number(i);;
    if(i < (argCount-1))
      {
      callArgs += ",";
      }
    }

  QString shortName = cname;
  QString name = typeName();
  if(shortName.length())
    {
    name += "." + shortName;
    }

  QString nativeName = addDartNativeLookup(_typeName, "_ctor_" + shortName, extraArgs + argCount, (Dart_NativeFunction)ctor);
  _functionSource += name + "(" + callArgs + ") {" + nativeName + "(" + callArgs + "); } \n";
  _functionSource += "void " + nativeName + "(" + callArgs + ") native \"" + nativeName + "\";\n";
#else
  xAssertFail();
  //(*::prototype(_prototype))->SetAccessor(v8::String::New(cname), (v8::AccessorGetter)getter, (v8::AccessorSetter)setter);

  xAssert(fn);
#endif
  }

void InterfaceBase::addProperty(
    const char *cname,
    GetterFn getter,
    FunctionDart fnGetter,
    SetterFn setter,
    FunctionDart fnSetter)
  {
#ifdef X_DART
  QString name = cname;
  if(fnGetter)
    {
    _functionSource += "Dynamic get " + name + "() => null;\n";
    }
  if(fnSetter)
    {
    _functionSource += "set " + name + "(var a) => null;\n";
    }
#else
  (*::prototype(_prototype))->SetAccessor(v8::String::New(cname), (v8::AccessorGetter)getter, (v8::AccessorSetter)setter);
#endif
  }

void InterfaceBase::addFunction(const char *cname,
                                 xsize extraArgs,
                                 xsize argCount,
                                 XScript::Function fn,
                                 XScript::FunctionDart fnDart)
  {
#ifdef X_DART
  QString callArgs;
  for(size_t i = 0; i < argCount; ++i)
    {
    callArgs += "_" + QString::number(i);;
    if(i < (argCount-1))
      {
      callArgs += ",";
      }
    }

  const char* leadIn = "Dynamic ";
  if(extraArgs == 0)
  {
    leadIn = "static Dynamic";
  }

  QString name = cname;
  QString resolvedName = addDartNativeLookup(_typeName, name, extraArgs + argCount, (Dart_NativeFunction)fnDart);
  _functionSource += leadIn + name + "(" + callArgs + ") native \"" + resolvedName + "\";\n";
#else
  v8::Handle<v8::FunctionTemplate> fnTmpl = ::v8::FunctionTemplate::New((v8::InvocationCallback)fn);
  (*::prototype(_prototype))->Set(v8::String::New(cname), fnTmpl->GetFunction());
#endif
  }

void InterfaceBase::addFunction(const char *cname,
                 xsize extraArgs,
                 xsize argCount,
                 XScript::Function fn,
                 XScript::FunctionDart fnDart,
                 int userData)
  {
#ifdef X_DART
  QString callArgs;
  for(size_t i = 0; i < argCount; ++i)
    {
    callArgs += "_" + QString::number(i);;
    if(i < (argCount-1))
      {
      callArgs += ",";
      }
    }

  const char* leadIn = "Dynamic ";
  if(extraArgs == 0)
    {
    leadIn = "static Dynamic";
    }

  xAssertFail(); // userData?

  QString name = cname;
  QString resolvedName = addDartNativeLookup(_typeName, name, extraArgs + argCount, (Dart_NativeFunction)fnDart);
  _functionSource += leadIn + name + "(" + callArgs + ") native \"" + resolvedName + "\";\n";
#else
  v8::Handle<v8::FunctionTemplate> fnTmpl = ::v8::FunctionTemplate::New((v8::InvocationCallback)fn);


  v8::Handle<v8::Integer> id = v8::Integer::New(userData);
  fnTmpl->Set(v8::String::New("0"), id);

  (*::prototype(_prototype))->Set(v8::String::New(cname), fnTmpl);
#endif
  }

void InterfaceBase::setIndexAccessor(IndexedGetter g, XScript::FunctionDart dart)
  {
#ifdef X_DART
  QString resolvedName = addDartNativeLookup(_typeName, "_indexedAccessor", 2, (Dart_NativeFunction)dart);
  _functionSource += "Dynamic operator [](idx) native \"" + resolvedName + "\";\n";
#else
  (*::prototype(_prototype))->SetIndexedPropertyHandler((v8::IndexedPropertyGetter)g);
#endif
  }

void InterfaceBase::setNamedAccessor(NamedGetter g)
  {
#ifdef X_DART
#else
  (*::prototype(_prototype))->SetFallbackPropertyHandler((v8::NamedPropertyGetter)g);
#endif
  }

#endif

#if 0
void InterfaceBase::addClassTo(const QString &thisClassName, const Object &dest) const
  {
#ifdef X_DART
  xAssertFail();
#else
  getV8Internal(dest)->Set(v8::String::NewSymbol(thisClassName.toAscii().constData()), getV8Internal(constructorFunction()));
#endif
  }
#endif

void InterfaceBase::addChildInterface(int typeId, UpCastFn fn)
  {
  xAssert(!_upcasts.contains(typeId));
  _upcasts.insert(typeId, fn);
  }
/*
void *InterfaceBase::prototype()
  {
  return _prototype;
  }
*/

Eks::UnorderedMap<int, InterfaceBase*> _interfaces;
void registerInterface(int id, int nonPtrId, InterfaceBase *interface)
  {
  xAssert(!_interfaces.contains(id));
  xAssert(!_interfaces.contains(nonPtrId));
  xAssert(id != QVariant::Invalid);

  _interfaces.insert(id, interface);
  if(nonPtrId != QVariant::Invalid)
    {
    _interfaces.insert(nonPtrId, interface);
    }
  }

void registerInterface(InterfaceBase *interface)
  {
  registerInterface(interface->typeId(), interface->nonPointerTypeId(), interface);
  }

InterfaceBase *findInterface(int id)
  {
#ifdef X_DEBUG
  QVariant::Type type = (QVariant::Type)id;
  (void)type;
  const char *typeName = QMetaType::typeName(type);
  (void)typeName;
#endif

  xAssert(id != QVariant::Invalid);
  InterfaceBase *base = _interfaces.value(id);
  xAssert(!base || base->isSealed());
  return base;
  }

}
