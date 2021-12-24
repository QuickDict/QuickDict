import QtQuick 2.15
import com.quickdict.components 1.0
import QtQuick.Layouts 1.15

Dict {
    name: qsTr("DeepL Translate")
    description: qsTr("DeepL Translate uses data from https://api-free.deepl.com.")
    property url url: "https://api-free.deepl.com/v2/translate"
    property string authKey
    property string tl
    property var delegate: Component {
        ColumnLayout {
            property alias modelData: text.modelData

            TextEdit {
                id: text
                property var modelData
                text: modelData.translation.text
                wrapMode: Text.Wrap
                font.pixelSize: sp(12)
                Layout.leftMargin: dp(8)
                Layout.fillWidth: true
                readOnly: true
                selectByMouse: true
            }
        }
    }

    onQuery: {
        axios.post(url,
                    objectToQueryString({
                    text: text,
                    auth_key: authKey,
                    target_lang: tl
                    })
            )
            .then(function (response) {
                if (response.data.translations) {
                    let result = {"engine": name, "text": "guess", "type": "lookup", "translation": response.data.translations[0]}
                    queryResult(result)
                }
            })
            .catch(function (error) {
                console.log("DeepL Translate:", error)
            })
    }

    Component.onCompleted: {
        qd.targetLanguageChanged.connect((lang) => {
                tl = lang.substr(0, 2).toUpperCase();
            })
    }

    function objectToQueryString(obj) {
        var str = [];
        for (var p in obj) {
            if (obj.hasOwnProperty(p))
                str.push(encodeURIComponent(p) + "=" + encodeURIComponent(obj[p]))
        }
        return str.join("&");
    }
}
