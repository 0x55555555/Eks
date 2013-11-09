import "../EksBuild" as Eks;

Eks.Library {
  name: "EksScript"
  toRoot: "../../"

  property var engines: [ ] // [ "Dart", "Javascript" ]

  files: [ "*.h", "*.cpp", "Engines/*" ]

  Depends { name: "Qt.network" }
  Depends { name: "Qt.widgets" }
  Depends { name: "Qt.gui" }

  Depends { name: "EksCore" }

  Group {
    name: "Dart"
    condition: engines.indexOf("Dart") != -1

    cpp.defines: base.concat( [ "X_SCRIPT_ENGINE_ENABLE_DART" ] )

    cpp.libraryPaths: base.concat( [ "dart" ] )
    cpp.dynamicLibraries: base.concat( [
      "libdart_builtin",
      "libdart_withcore",
      "libdart_lib_withcore",
      "libdouble_conversion",
      "libdart_vm",
      "libjscre"
    ] )
  }

  Group {
    name: "Javascript"
    condition: engines.indexOf("Javascript") != -1

    cpp.defines: base.concat( [ "X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT" ] )
  }

  Export {
    Depends { name: "cpp" }
    Depends { name: "EksCore" }	
    cpp.includePaths: ["."]
  }
}
