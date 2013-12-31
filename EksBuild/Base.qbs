import qbs

Product {
  Depends { name: "cpp" }

  property bool clang: true
  property bool debug: true

  property string toRoot: ""

  files: [ "include/**/*", "src/**/*" ]
  cpp.includePaths: base.concat([ "include" ])

  cpp.treatWarningsAsErrors: true

  property var commonDefines: [ "X_CPPOX_SUPPORT", name.toUpperCase() + "_BUILD" ]
  Properties {
    condition: debug
    cpp.defines: commonDefines.concat( [ "X_DEBUG" ] )
  }
  Properties {
    condition: !debug
    cpp.defines: commonDefines
  }

  Properties {
    condition: clang
    cpp.cxxFlags: base.concat( [ "-std=c++11", "-stdlib=libc++" ] )
    cpp.minimumOsxVersion: "10.9"
  }

  /*Properties {
    condition: true //qbs.toolchain == "msvc"
	cpp.cxxFlags: base.concat( [ "/FS" ] )
  }*/

}
