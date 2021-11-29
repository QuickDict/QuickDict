import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    name: qsTr("Example Dict")
    description: qsTr("Example Dict demonstrates how to write your own dict plugins.")
    property var delegate: Component {
        Text {
            id: text
            property var modelData
            text: modelData.result
        }
    }

    onQuery: {
        let result = {"engine": name, "text": text, "result": "This is the result of Example Dict.", "type": "lookup"}
        queryResult(result)
    }

    Component.onCompleted: {
        qd.registerDict(this)
    }
}
