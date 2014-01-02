import qbs 1.0
import "Utils.js" as Utils;

Product {
  qbsSearchPaths: "../EksBuild"
  Depends { name: "buildtools" }

  property string toRoot: ""

  files: [ "include/**/*", "src/**/*" ]

  Depends { name: "cpp" }

  cpp.includePaths: base.concat([ "include" ])
  cpp.defines: base.concat( [ name.toUpperCase() + "_BUILD" ] )
}
