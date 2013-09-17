import qbs 1.0
//import "../EksBuild" as Eks;

DynamicLibrary {
    name: "EksGui"
    //toRoot: "../../"

    Depends { name: "cpp" }
    files: [ "include/**/*", "src/**/*" ]

    Depends {
        name: "EksCore"
    }

    /*Export {
        cpp.includePaths: ["include"]

        Depends {
            name: "EksCore"
        }

        Depends {
            name: "Qt"
            submodules: [ "gui", "widgets" ]
        }
    }*/
}
