import QtQuick 2.15
import com.quickdict.components 1.0
import "data.js" as Data

Dict {
    name: qsTr("MockDict")
    enabled: false
    description: qsTr("MockDict uses mockup data.")

    onQuery: {
        for (const source of Data.sources) {
            queryResult(source)
        }
    }

    Component.onCompleted: {
        qd.registerDict(this)
    }
}
