import "../EksBuild" as Eks;

Eks.Application {
  name: "EksDebugger"
  toRoot: "../../"

  files: [ "*.h", "*.cpp" ]

  Depends { name: "Qt.widgets" }
  Depends { name: "Qt.gui" }

  Depends { name: "EksCore" }
  Depends { name: "EksDebug" }
}
