import QtQuick 2.15
import com.quickdict.components 1.0
import "data.js" as Data

Dict {
    id: mockDict
    name: qsTr("MockDict")
    enabled: false
    description: qsTr("MockDict uses mockup data.")

    onQuery: {
        for (const source of Data.sources) {
            mockDict.queryResult(source)
        }
    }

    Component.onCompleted: {
        qd.registerDict(mockDict)
    }
}
