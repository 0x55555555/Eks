import qbs 1.0
import "Utils.js" as Utils;

Base {
  property bool debug: qbs.buildVariant == "debug"
  property bool staticLib: false

  type: staticLib ? "staticlibrary" : "dynamiclibrary"

  Depends { name: "cpp" }
  files: [ "include/**/*", "src/**/*" ]
  cpp.includePaths: base.concat( [ "include" ] )

  moduleSearchPaths: toRoot

  /*Group {

    : qbs.buildVariant == "debug"
  }*/

  property var commonDefines: [ name.toUpperCase() + "_BUILD" ]
  Properties {
    condition: debug
    cpp.defines: commonDefines.concat( [ "X_DEBUG" ] )
  }
  Properties {
    condition: !debug
    cpp.defines: commonDefines
  }
}
