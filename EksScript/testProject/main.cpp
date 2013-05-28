#include "XScriptEngine.h"
#include "XScriptInterface.h"
#include "XScriptValue.h"
#include "XScriptSource.h"
#include "XAssert"
#include "XScriptFunctionWrapper.h"

struct Nummer
  {
  double num;

  Nummer(float n) : num(n) {}

  float crapSalad(float newNum)
    {
    num += newNum;

    return num;
  }

  static float cake(int)
    {
    return 0.0f;
    }

  float cake2(int, float) const
    {
    return 0.0f;
    } 
  };

void thing()
  {
  }

namespace XScript
{

void defineType(Interface<Nummer> *ifc)
  {
  typedef detail::FunctionList<
    X_BIND_FUNCTION(&Nummer::crapSalad),
    X_BIND_FUNCTION(&Nummer::cake),
    X_BIND_FUNCTION(&thing),
    X_BIND_FUNCTION(&Nummer::cake2)
  > FunctionList;

  ifc->build<FunctionList>("Nummer");
}

}

X_SCRIPTABLE_TYPE(Nummer)

int main(int , char* [])
{
  /*auto argsA = List::at<0>::Traits::ArgData(5.0f);
  auto argsB = List::at<1>::Traits::ArgData(5);
  auto argsC = List::at<2>::Traits::ArgData();
  auto argsD = List::at<3>::Traits::ArgData(5, 5);

  Nummer n(0.0);
  float val = 0.0f;

  List::at<0>::call(&val, &n, &argsA);
  List::at<1>::call(&val, 0, &argsB);
  List::at<2>::call(0, 0, &argsC);
  List::at<3>::call(&val, 0, &argsD);*/

  XScript::Engine::initiate(false);

  auto nummer = XScript::Interface<Nummer>::find();
  XScript::Engine::addInterface(nummer);
  
  const char* kScriptChars =
  "#import(\"Nummer\");"
  "String main() {\n"
  "  Nummer obj = new Nummer(45.5);\n"
  "  obj.crapSalad(5.5);"
  "  return obj.crapSalad(5.0);"
  "}\n";

  XScript::Source src("txt.txt", kScriptChars);

  XScript::Value val = src.run();

  qDebug() << val.toString();

  return 0;
}
