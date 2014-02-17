import "EksBuild" as Eks;

Eks.SubModule {
    name: "Eks"
    toRoot: "../"

    qbsSearchPaths: "EksCore/BuildModules"
    moduleSearchPaths: "EksCore/BuildModules/modules"

    references: [
        "EksCore/EksCore.qbs",
        "EksGui/EksGui.qbs",
        "Eks3D/Eks3D.qbs",
        "EksScript/EksScript.qbs",
        //"EksReflex/EksReflex.qbs",
        "EksDebug/EksDebug.qbs",
        //"EksDebugger/EksDebugger.qbs",
    ]
}
