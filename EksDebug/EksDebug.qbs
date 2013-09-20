import "../EksBuild" as Eks;

Eks.Library {
  name: "EksDebug"
  toRoot: "../../"

  Depends { name: "Qt.network" }

  Export {
    cpp.includePaths: ["include"]
  }
}
