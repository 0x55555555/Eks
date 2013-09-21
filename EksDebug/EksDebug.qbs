import "../EksBuild" as Eks;

Eks.Library {
  name: "EksDebug"
  toRoot: "../../"

  files: [ "*.h", "*.cpp" ]

  Depends { name: "Qt.network" }
  Depends { name: "EksCore" }

  Export {
    Depends { name: "cpp" }
    cpp.includePaths: ["."]
  }
}
