import qbs
import qbs.File
import qbs.TextFile
import "qbs/imports/QbsUtl/qbsutl.js" as QbsUtl

Project {
    minimumQbsVersion: "1.20.0"
    qbsSearchPaths: ["qbs"]

    readonly property string sodiumVersion: "1.0.18"
    readonly property bool   useSystemSodium: false

    readonly property var projectVersion: projectProbe.projectVersion
    readonly property string projectGitRevision: projectProbe.projectGitRevision

    Probe {
        id: projectProbe
        property var projectVersion;
        property var databaseVersion;
        property string projectGitRevision;

        readonly property string projectBuildDirectory:  project.buildDirectory
        readonly property string projectSourceDirectory: project.sourceDirectory

        configure: {
            projectVersion = QbsUtl.getVersions(projectSourceDirectory + "/VERSION");
            projectGitRevision = QbsUtl.gitRevision(projectSourceDirectory);
            if (File.exists(projectBuildDirectory + "/package_build_info"))
                File.remove(projectBuildDirectory + "/package_build_info")
        }
    }

    property var cppDefines: {
        var def = [
            "APPLICATION_NAME=\"ToxRealay\"",
            "VERSION_PROJECT=\"" + projectVersion[0] + "\"",
            "VERSION_PROJECT_MAJOR=" + projectVersion[1],
            "VERSION_PROJECT_MINOR=" + projectVersion[2],
            "VERSION_PROJECT_PATCH=" + projectVersion[3],
            "GIT_REVISION=\"" + projectGitRevision + "\"",
            "QDATASTREAM_VERSION=QDataStream::Qt_5_12",
            "PPROTO_VERSION_LOW=0",
            "PPROTO_VERSION_HIGH=0",
          //"PPROTO_JSON_SERIALIZE",
            "PPROTO_QBINARY_SERIALIZE",
            "LOGGER_LESS_SNPRINTF",
            "TOX_MESSAGE_SIGNATURE=\"TOXRELAY\"",
            "CONFIG_DIR=\"/etc/toxrelay\"",
            "VAROPT_DIR=\"/var/opt/toxrelay\"",
        ];
        return def;
    }

    property var cxxFlags: [
        "-ggdb3",
        "-Wall",
        "-Wextra",
        "-Wswitch-enum",
        "-Wdangling-else",
        "-Wno-unused-parameter",
        "-Wno-variadic-macros",
        "-Wno-vla",
    ]
    property string cxxLanguageVersion: "c++17"
}
