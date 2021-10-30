import QtQuick 2.15
import QtQuick.Controls 2.15
import "axios.min.js" as Axios

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: stackView.depth > 1 ? stackView.currentItem.title + qsTr(" - QuickDcit") : qsTr("QuickDcit")

    header: ToolBar {
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            text: stackView.currentItem.title
            anchors.centerIn: parent
        }
    }

    Drawer {
        id: drawer
        width: window.width * 0.33
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Settings")
                width: parent.width
                onClicked: {
                    stackView.push("Settings.ui.qml")
                    drawer.close()
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: "Main.ui.qml"
        anchors.fill: parent
    }

    Connections {
        target: qd.ocrEngine
        function onStarted() {
            console.log("OcrEngine started!")
        }
        function onStopped() {
            console.log("OcrEngine stopped!")
        }
    }

    Component.onCompleted: {
        console.log(qd, qd.ocrEngine, qd.ocrEngine.isRunning())
        console.log("Start request")
        axios.get("http://localhost:3000/hack")
            .then(function (response) {
                console.log("Response", response.status)
                /* console.log(JSON.stringify(response.data)) */
                console.log(response.data["list"][0]["definition"])
            })
            .catch(function (error) {
                console.log("Error")
            })
            .then(function() {
                console.log("Finally")
            })
    }
}
