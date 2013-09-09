import qbs 1.0
import "Utils.js" as Utils;

Base {
    type: "dynamiclibrary"
    property bool debug: qbs.buildVariant == "debug"

    Depends { name: "cpp" }
    files: [ "include/**/*", "src/**/*" ]
    cpp.includePaths: ["include"]

    moduleSearchPaths: toRoot

    /*Group {

        condition: qbs.buildVariant == "debug"
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
