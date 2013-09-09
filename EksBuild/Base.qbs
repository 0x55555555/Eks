import qbs 1.0

Product {
    property bool debug: qbs.buildVariant == "debug"
    property string toRoot: ""

    moduleSearchPaths: toRoot

    property var commonDefines: [ name.toUpperCase() + "_BUILD" ]
}
