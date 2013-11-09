import "../EksBuild" as Eks;

Eks.Library {
  name: "EksGui"
  toRoot: "../../"

  files: [ "include/**/*", "src/**/*" ]

  Depends {
    name: "EksCore"
  }

  Depends {
    name: "Qt"
    submodules: [ "gui", "widgets" ]
  }

  Export {
    cpp.includePaths: ["include"]

    Depends { name: "cpp" }
    Depends { name: "EksCore" }	
    Depends {
      name: "Qt"
      submodules: [ "gui", "widgets" ]
    }
}
}
