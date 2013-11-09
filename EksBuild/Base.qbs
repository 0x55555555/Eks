import qbs

Product {
  Depends { name: "cpp" }
  Depends { name: "qbs" }

  property bool debug: true

  property string toRoot: ""

  files: [ "include/**/*", "src/**/*" ]
  cpp.includePaths: base.concat( [ "include" ] )

  cpp.treatWarningsAsErrors: true

  property var commonDefines: [ name.toUpperCase() + "_BUILD" ]
  Properties {
    condition: debug
    cpp.defines: commonDefines.concat( [ "X_DEBUG" ] )
  }
  Properties {
    condition: !debug
    cpp.defines: commonDefines
  }
  
  Properties {
    condition: true //qbs.toolchain == "msvc"
	cpp.cxxFlags: base.concat( [ "/FS" ] )
  }
}
