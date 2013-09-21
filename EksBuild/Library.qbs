import qbs 1.0
import "Utils.js" as Utils;

Base {
  property bool staticLib: false

  type: staticLib ? "staticlibrary" : "dynamiclibrary"
}
