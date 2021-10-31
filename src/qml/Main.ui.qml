import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    title: qsTr("QuickDcit")

    ScrollView {
        id: scrollView
        clip: true
        anchors.fill: parent

        Column {
            Repeater {
                id: repeater

                Text {
                    text: modelData.definition
                    width: scrollView.width
                    wrapMode: Text.WrapAnywhere
                    font.pixelSize: Qt.application.font.pixelSize * 1.6
                }
            }
        }

        Connections {
            target: qd.dictService
            function onQueryResult(result) {
                repeater.model = result.list
            }
        }
    }
}
