import "../EksBuild" as Eks;

Eks.Library {
  name: "EksGui"
  toRoot: "../../"

  Depends {
    name: "EksCore"
  }

  Depends {
    name: "Eks3D"
  }

  cpp.defines: base.concat( [ "X_ENABLE_GL_RENDERER" ] )

  Depends {
    name: "Qt"
    submodules: [ "gui", "widgets", "opengl" ]
  }

  Export {
    Depends { name: "cpp" }
    cpp.includePaths: ["include"]

    Depends { name: "EksCore" }
    Depends { name: "Eks3D" }
    Depends {
      name: "Qt"
      submodules: [ "gui", "widgets", "opengl" ]
    }
  }
}
