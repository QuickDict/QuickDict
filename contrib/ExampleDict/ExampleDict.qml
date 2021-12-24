import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    name: qsTr("Example Dict")
    description: qsTr("Example Dict demonstrates how to write your own dict plugins.")
    delegate: Component {
        TextEdit {
            id: text
            property var modelData
            text: modelData.result
            font.pixelSize: sp(12)
            wrapMode: Text.Wrap
        }
    }

    onQuery: {
        let result = {"engine": name, "text": text, "result": "This is the result of Example Dict.", "type": "lookup"}
        queryResult(result)
    }
}
