import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    id: googleTranslate
    name: qsTr("Google Translate")
    enabled: true
    description: qsTr("DictdDict uses data from https://translate.google.com.")
    property url url: "https://translate.google.com/#view=home&op=translate&sl=auto&tl=en&text="

    onQuery: {
        let result = {"engine": name, "text": text, "type": "translation", "url": url + text}
        googleTranslate.queryResult(result)
    }

    Component.onCompleted: {
        qd.registerDict(googleTranslate)
    }
}
