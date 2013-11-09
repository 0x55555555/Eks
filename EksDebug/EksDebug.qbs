import "../EksBuild" as Eks;

Eks.Library {
  name: "EksDebug"
  toRoot: "../../"

  Depends { name: "Qt.network" }
  Depends { name: "EksCore" }

  Export {
    Depends { name: "cpp" }
    cpp.includePaths: ["include"]
  }
}
