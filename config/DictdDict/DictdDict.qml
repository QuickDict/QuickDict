import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    id: dictdDict
    name: qsTr("DictdDict")
    enabled: true
    description: qsTr("DictdDict uses data from https://dict.org.")
    property url url: "https://dict.org/bin/Dict?Form=Dict2&Database=*&Query="

    onQuery: {
        let result = {"engine": name, "text": text, "type": "translation", "url": url + text}
        dictdDict.queryResult(result)
    }

    Component.onCompleted: {
        qd.registerDict(dictdDict)
    }
}