import "../EksBuild" as Eks;

Eks.Library {
  name: "EksReflex"
  toRoot: "../../"

  files: [ "*.h", "*.cpp" ]

  Depends { name: "EksCore" }

  Export {
    Depends { name: "cpp" }
    Depends { name: "EksCore" }	
    cpp.includePaths: [ "." ]
  }
}
