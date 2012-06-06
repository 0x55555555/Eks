#define _ITERATOR_DEBUG_LEVEL 0

#include "XScriptEngine.h"
#include "XInterface.h"
#include "XAssert"

struct Nummer
{
  double num;

  Nummer(float n) : num(n) {}

  QString crapSalad(float newNum)
  {
    num += newNum;

    return QString::number(num);
  }
};

X_SCRIPTABLE_TYPE(Nummer)

int main(int a, char* b[])
{
  XScriptEngine::initiate();

  XScriptEngine eng(false);

  XInterface<Nummer>* nummer = XInterface<Nummer>::create("Nummer");
  nummer->addConstructor<Nummer* (float)>();
  nummer->addMethod<QString(float), &Nummer::crapSalad>("crapSalad");
  nummer->seal();

  eng.addInterface(nummer);
  
  const char* kScriptChars =
  "#import(\"Nummer\");"
  "String main() {\n"
  "  Nummer obj = new Nummer(45.5);\n"
  "  obj.crapSalad(5.5);"
  "  return obj.crapSalad(5.0).toStringAsFixed(5);"
  "}\n";

  XScriptValue val = eng.run(kScriptChars);

  qDebug() << val.toString();

  return 0;
}
