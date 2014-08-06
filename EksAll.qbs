import "EksBuild" as Eks;

Eks.SubModule {
  name: "Eks"
  toRoot: "../"

  references: [
    "EksCore/EksCore.qbs",
    "EksGui/EksGui.qbs",
    "Eks3D/Eks3D.qbs",
    "Eks3D/test/test.qbs",
    "Eks3D/examples/example.qbs",
    "EksDebug/EksDebug.qbs",
    "EksConcept/EksConcept.qbs",
    "EksConcept/test/test.qbs",
  ]

  //"EksScript/EksScript.qbs",
  //"EksReflex/EksReflex.qbs",
  //"EksDebugger/EksDebugger.qbs",
}
