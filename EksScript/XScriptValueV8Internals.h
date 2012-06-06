#ifndef XScriptValueV8INTERNALS_H
#define XScriptValueV8INTERNALS_H

#include "v8.h"

class XScriptValue;
class XScriptObject;
class XInterfaceBase;
class XScriptFunction;
class XScriptEngine;

XScriptValue fromHandle(v8::Handle<v8::Value>);
XScriptObject fromObjectHandle(v8::Handle<v8::Object>);
XScriptFunction fromFunction(v8::Handle<v8::Function>);

v8::Handle<v8::Value> getV8Internal(const XScriptValue&);
v8::Handle<v8::Object> getV8Internal(const XScriptObject &o);
v8::Handle<v8::ObjectTemplate> getV8Internal(XInterfaceBase *o);
v8::Handle<v8::Function> getV8Internal(const XScriptFunction&);
v8::Handle<v8::Context> getV8EngineInternal();

v8::Handle<v8::Value> *getV8Internal(const XScriptValue*);

v8::Handle<v8::ObjectTemplate> getGlobalTemplate(XScriptEngine *);

#endif // XScriptValueV8INTERNALS_H
