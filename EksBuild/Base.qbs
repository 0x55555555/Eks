import qbs 1.0
import "Utils.js" as Utils;

Product {
  moduleSearchPaths: "../EksBuild/modules"
  qbsSearchPaths: "../EksBuild"
  Depends { name: "buildtools" }

  property path toRoot: ""

  files: [ "include/**/*", "src/**/*" ]

  Depends { name: "cpp" }

  cpp.includePaths: base.concat([ "include" ])
  cpp.defines: base.concat( [ name.toUpperCase() + "_BUILD" ] )
}
