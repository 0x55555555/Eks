import "../EksBuild" as Eks;

Eks.Application {
  name: "EksDebugger"
  toRoot: "../../"

  files: [ "*.h", "*.cpp" ]

  Depends { name: "Qt.network" }

  Depends { name: "EksCore" }
  Depends { name: "EksDebug" }
}
