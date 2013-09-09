import "../EksBuild" as Eks;

Eks.Library {
    name: "EksGui"
    toRoot: "../../"

    Depends {
        name: "EksCore"
    }

    Export {
        cpp.includePaths: ["include"]

        Depends {
            name: "EksCore"
        }

        Depends {
            name: "Qt"
            submodules: [ "gui", "widgets" ]
        }
    }
}
