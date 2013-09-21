import "../EksBuild" as Eks;

Eks.Library {
  name: "EksDebug"
  toRoot: "../../"

  files: [ "*.h", "*.cpp" ]

  Depends { name: "Qt.network" }

  Export {
    cpp.includePaths: ["."]
  }
}
