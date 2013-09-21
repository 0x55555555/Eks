import qbs 1.0

Product {
  Depends { name: "cpp" }

  property bool debug: cpp.debugInformation

  property string toRoot: ""

  files: [ "include/**/*", "src/**/*" ]
  cpp.includePaths: base.concat( [ "include" ] )

  cpp.treatWarningsAsErrors: true

  moduleSearchPaths: toRoot

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
