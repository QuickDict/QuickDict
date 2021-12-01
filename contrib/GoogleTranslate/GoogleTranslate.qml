import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    name: qsTr("Google Translate")
    description: qsTr("Google Translate uses data from https://translate.google.com.")
    property url url: "https://translate.google.com/#view=home&op=translate&sl=auto&tl=(tl)&text="
    property string tl

    onQuery: {
        let result = {"engine": name, "text": text, "type": "translation", "url": String(url).replace("(tl)", tl) + text}
        queryResult(result)
    }

    Component.onCompleted: {
        qd.registerDict(this)
        qd.targetLanguageChanged.connect((lang) => {
                if (lang.startsWith("zh_"))
                    tl = lang.replace("_", "-")
                else
                    tl = lang.replace(/_.+/, "")
            })
    }
}
