import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    name: qsTr("Google Translate")
    description: qsTr("DictdDict uses data from https://translate.google.com.")
    property string url: "https://translate.google.com/#view=home&op=translate&sl=auto&tl={tl}&text="

    onQuery: {
        let tl = convertLangName(qd.configCenter.value("/lang/tl", "en_US"))
        let result = {"engine": name, "text": text, "type": "translation", "url": url.replace("{tl}", tl) + text}
        queryResult(result)
    }

    Component.onCompleted: {
        qd.registerDict(this)
    }

    function convertLangName(name) {
        if (name.startsWith("zh_"))
            name = name.replace("_", "-")
        else
            name = name.replace(/_.+/, "")
        return name
    }
}
