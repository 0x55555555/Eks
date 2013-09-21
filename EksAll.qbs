import "EksBuild" as Eks;

Eks.SubModule {
    name: "Eks"

    references: [
        "EksCore/EksCore.qbs",
        "EksGui/EksGui.qbs",
        "Eks3D/Eks3D.qbs",
        "EksScript/EksScript.qbs",
        "EksDebug/EksDebug.qbs",
        "EksDebugger/EksDebugger.qbs",
    ]
}
