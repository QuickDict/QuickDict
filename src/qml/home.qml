import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1
import com.quickdict.components 1.0
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

    Dict {
        id: urbanDict
        onQuery: {
            /* json-server with https://www.urbandictionary.com/define.php?term=hack" */
            axios.get("http://localhost:3000/" + text)
                .then(function (response) {
                    urbanDict.queryResult(JSON.parse(JSON.stringify(response.data)))
                })
                .catch(function (error) {
                    console.log("UrbanDict:", error)
                })
        }
        onQueryResult: {
            /* console.log(JSON.stringify(result)) */
        }

        Component.onCompleted: {
            console.log("UrbanDict: loaded")
            qd.dictService.registerDict(urbanDict)
        }
    }

    Dict {
        id: dictdDict
        onQuery: {
            url = "https://dict.org/bin/Dict?Form=Dict2&Database=*&Query=" + text
            window.show()
            window.raise()
            window.requestActivate()
        }

        Component.onCompleted: {
            console.log("DictdDict: loaded")
            qd.dictService.registerDict(dictdDict)
            url = "https://dict.org/bin/Dict?Form=Dict2&Database=*"
        }
        property url url
    }

    SystemTrayIcon {
        id: systemTrayIcon
        visible: true
        icon.source: "qrc:/images/QuickDict-32x32.png"

        menu: Menu {
            MenuItem {
                text: qsTr("Show")
                onTriggered: {
                    window.show()
                    window.raise()
                    window.requestActivate()
                }
            }
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }
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
        /* urbanDict.query("hack") */
    }
}
