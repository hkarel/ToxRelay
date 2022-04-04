import qbs

Product {
    name: "QRcode"
    targetName: "qrcode"

    type: "staticlibrary"
    Depends { name: "cpp" }

     cpp.cxxFlags: [
         "-ggdb3",
         "-Wall",
         "-Wextra",
         "-Wno-unused-parameter",
     ]
     cpp.includePaths: ["QR-Code-generator/cpp"]
     cpp.cxxLanguageVersion: "c++17"

     files: [
         "QR-Code-generator/cpp/qrcodegen.cpp",
         "QR-Code-generator/cpp/qrcodegen.hpp",
     ]
     Export {
         Depends { name: "cpp" }
         cpp.systemIncludePaths: ["QR-Code-generator/cpp/"]
     }
}
