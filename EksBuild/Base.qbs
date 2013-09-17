import qbs 1.0

Product {
    property bool debug: qbs.buildVariant == "debug"
    property string toRoot: ""

    moduleSearchPaths: toRoot

    properaty var commonDefines: [ name.toUpperCase() + "_BUILD" ]
}
