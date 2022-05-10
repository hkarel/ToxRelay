import qbs
import QbsUtl
import ProbExt

Product {
    name: "ToxRelay"
    targetName: "toxrelay"
    condition: true

    type: "application"
    destinationDirectory: "./bin"

    Depends { name: "cpp" }
    Depends { name: "PProto" }
    Depends { name: "QRcode" }
    Depends { name: "SharedLib" }
    Depends { name: "ToxCore" }
    Depends { name: "ToxFunc" }
    Depends { name: "UsbRelay" }
    Depends { name: "Yaml" }
    Depends { name: "lib.sodium" }
    Depends { name: "Qt"; submodules: ["core", "network"] }

    lib.sodium.version:   project.sodiumVersion
    lib.sodium.useSystem: project.useSystemSodium

    cpp.defines: project.cppDefines
    cpp.cxxFlags: project.cxxFlags
    cpp.cxxLanguageVersion: project.cxxLanguageVersion

    cpp.includePaths: [
        "./",
        "../",
    ]

    cpp.systemIncludePaths: QbsUtl.concatPaths(
        Qt.core.cpp.includePaths // Декларация для подавления Qt warning-ов
       ,lib.sodium.includePath
    )

    cpp.rpaths: QbsUtl.concatPaths(
        lib.sodium.libraryPath
       ,"/opt/toxrelay/lib"
     //,"$ORIGIN/../lib"
    )

    cpp.libraryPaths: QbsUtl.concatPaths(
        lib.sodium.libraryPath
    )

    cpp.dynamicLibraries: {
        var libs = [
            "pthread"
           ,"usb-1.0"
        ].concat(
            lib.sodium.dynamicLibraries
        );
        return libs;
    }

    files: [
        "commands/commands.cpp",
        "commands/commands.h",
        "tox/tox_net.cpp",
        "tox/tox_net.h",
        "toxrelay.cpp",
        "toxrelay.qrc",
        "toxrelay_appl.cpp",
        "toxrelay_appl.h",
    ]

} // Product
