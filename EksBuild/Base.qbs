import qbs 1.0
import "Utils.js" as Utils;

Product {
  qbsSearchPaths: "../EksBuild"
  Depends { name: "cpp" }
  Depends { name: "buildtools" }

  property path toRoot: ""

  files: [ "include/**/*", "src/**/*" ]

  cpp.includePaths: base.concat([ "include" ])

  Depends { name: "cpp" }
  cpp.treatWarningsAsErrors: true

  property var commonDefines: [
    "X_CPPOX_SUPPORT",
    name.toUpperCase() + "_BUILD"
  ]

  Properties {
    condition: buildtools.debug
    cpp.defines: commonDefines.concat( [ "X_DEBUG" ] )
  }
  Properties {
    condition: !buildtools.debug
    cpp.defines: commonDefines
  }

  Properties {
    condition: buildtools.osx || lbuildtools.inux
    cpp.cxxFlags: base.concat( [ "-std=c++11", "-stdlib=libc++" ] )
  }

  Properties {
    condition: buildtools.osx
    cpp.minimumOsxVersion: "10.9"
  }
}
