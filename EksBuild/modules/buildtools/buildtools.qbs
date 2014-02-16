import qbs 1.0

Module {
  property bool debug: qbs.buildVariant == "debug"

  property bool windows: qbs.targetOS == "windows"
  property bool linux: qbs.targetOS == "linux"
  property bool osx: !windows && !linux

  Depends { name: "cpp" }
  cpp.treatWarningsAsErrors: true

  property var commonDefines: [ "X_CPPOX_SUPPORT" ]
  Properties {
    condition: debug
    cpp.defines: commonDefines.concat( [ "X_DEBUG" ] )
  }
  Properties {
    condition: !debug
    cpp.defines: commonDefines
  }

  Properties {
    condition: osx || linux
    cpp.cxxFlags: base.concat( [ "-std=c++11", "-stdlib=libc++" ] )
  }

  Properties {
    condition: osx
    cpp.minimumOsxVersion: "10.9"
  }


  validate: {
  }
}

/*
Module {
    property string buildVariant: "debug"
    property bool enableDebugCode: buildVariant == "debug"
    property bool debugInformation: (buildVariant == "debug")
    property string optimization: (buildVariant == "debug" ? "none" : "fast")
    property string hostOS: ""
    property string pathListSeparator: hostOS.contains("windows") ? ";" : ":"
    property string pathSeparator: hostOS.contains("windows") ? "\\" : "/"
    property string profile
    property stringList toolchain
    property string architecture

    property string endianness
    PropertyOptions {
        name: "endianness"
        allowedValues: ["big", "little", "mixed"]
        description: "endianness of the target platform"
    }

    property bool install: false
    property string installDir
    property string installPrefix: ""
    property path sysroot

    PropertyOptions {
        name: "buildVariant"
        allowedValues: ['debug', 'release']
        description: "name of the build variant"
    }

    PropertyOptions {
        name: "optimization"
        allowedValues: ['none', 'fast', 'small']
        description: "optimization level"
    }

    validate: {
        if (!architecture) {    // ### don't warn but throw in 1.2
            print("WARNING: qbs.architecture is not set. "
                  + "You might want to re-run 'qbs detect-toolchains'.");
            return;
        }

        var architectureSynonyms = {
            "x86": ["i386", "i486", "i586", "i686", "ia32", "ia-32", "x86_32", "x86-32", "intel32"],
            "x86_64": ["x86-64", "x64", "amd64", "ia32e", "em64t", "intel64"],
            "ia64": ["ia-64", "itanium"]
        };

        for (var arch in architectureSynonyms) {
            if (architectureSynonyms[arch].contains(architecture.toLowerCase())) {
                throw "qbs.architecture '" + architecture + "' is invalid. " +
                      "You must use the canonical name '" + arch + "'";
            }
        }
    }
}
*/
