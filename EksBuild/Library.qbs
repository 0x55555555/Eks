import qbs 1.0
import "Utils.js" as Utils;

DynamicLibrary {
    property bool debug: qbs.buildVariant == "debug"

    Depends { name: "cpp" }
    files: [ "include/**/*", "src/**/*" ]

    /*Group {

        condition: qbs.buildVariant == "debug"
    }*/

    root: "../"
    thisRoot: {
        print(buildDirectory);
        throw "PORK";
    }

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
