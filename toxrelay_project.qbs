import qbs
import "toxrelay_base.qbs" as ToxRealayBase

ToxRealayBase {
    name: "ToxRealay (Project)"

    references: [
        "src/pproto/pproto.qbs",
        "src/qrcode/qrcode.qbs",
        //"src/rapidjson/rapidjson.qbs",
        "src/shared/shared.qbs",
        "src/toxcore/toxcore.qbs",
        "src/toxrelay/toxrelay.qbs",
        "src/usbrelay/usb_relay.qbs",
        "src/yaml/yaml.qbs",
        //"setup/package_build.qbs",
    ]
}
