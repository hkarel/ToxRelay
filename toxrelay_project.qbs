import qbs
import "toxrelay_base.qbs" as ToxRealayBase

ToxRealayBase {
    name: "ToxRealay (Project)"

    references: [
        //"src/pproto/pproto.qbs",
        //"src/rapidjson/rapidjson.qbs",
        "src/shared/shared.qbs",
        "src/toxcore/toxcore.qbs",
        //"src/toxrelay/toxrelay.qbs",
        "src/usbrelay/usbrelay.qbs",
        "src/yaml/yaml.qbs",
        //"setup/package_build.qbs",
    ]
}
