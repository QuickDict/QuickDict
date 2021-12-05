import QtQuick 2.15
import QtMultimedia 5.15

Text {
    property var modelData
    text: modelData.text
    visible: text ? true : false
    font.pixelSize: sp(12)
    color: Qt.rgba(0, 0, 0, 0.87)

    Loader {
        id: audioPlayerLoader
        active: typeof modelData.audio !== "undefined"
        sourceComponent: Audio {
            source: modelData.audio
        }
    }
    MouseArea {
        anchors.fill: parent
        enabled: typeof modelData.audio !== "undefined"
        cursorShape: Qt.PointingHandCursor

        onPressed: audioPlayerLoader.item.play()
    }
}
