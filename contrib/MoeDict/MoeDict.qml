import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    name: qsTr("MoeDict")
    description: qsTr("MoeDict uses data from https://www.moedict.tw.")
    property url url: "https://www.moedict.tw/uni/"

    onQuery: {
        axios.get(url + text)
            .then(function (response) {
                let data = response.data.heteronyms[0]
                let result = {"engine": name, "text": response.data.title, "type": "lookup"}
                result.phonetic = [{"text": `/${data.bopomofo}/`}, {"text": `/${data.bopomofo2}/`}]
                result.definitions = []
                let definitions = {}
                for (const entry of data.definitions) {
                    if (!(entry.type in definitions))
                        definitions[entry.type] = {"partOfSpeech": entry.type, "list": []}
                    let examples = entry.example ? entry.example : []
                    let quotes = entry.quote ? entry.quote : []
                    definitions[entry.type].list.push({"definition": entry.def, "examples": examples.concat(quotes)})
                }
                for (const e of Object.values(definitions)) {
                    result.definitions.push(e)
                }
                queryResult(result)
            })
            .catch(function (error) {
                console.log("MoeDict:", error)
            })
    }

    Component.onCompleted: {
        qd.registerDict(this)
    }
}
