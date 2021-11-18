import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1
import com.quickdict.components 1.0
import "data.js" as Data
import "axios.min.js" as Axios

ApplicationWindow {
    id: window
    title: stackView.depth > 1 ? stackView.currentItem.title + qsTr(" - QuickDcit") : qsTr("QuickDcit")
    width: 1280
    height: 720
    color: "#f6f3ed"

    header: ToolBar {
        font.pixelSize: sp(20)
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }
        TextField {
            id: textField
            placeholderText: qsTr("type word to look up...")
            visible: stackView.depth <= 1
            anchors {
                left: toolButton.right
                right: parent.right
                margins: dp(20)
            }

            Monitor {
                id: textFieldMonitor

                Component.onCompleted: {
                    qd.monitorService.registerMonitor(textFieldMonitor)
                    console.log("TextFieldMonitor: loaded")
                }
            }

            onAccepted: {
                textFieldMonitor.query(text)
            }
        }
        Label {
            text: stackView.currentItem.title
            visible: stackView.depth > 1
            anchors.centerIn: parent
        }
    }

    Drawer {
        id: drawer
        width: dp(256)
        height: window.height
        font.pixelSize: sp(14)

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("WebView")
                width: parent.width
                onClicked: {
                    stackView.push("WebView.ui.qml")
                    drawer.close()
                }
            }
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
            qd.dictService.registerDict(urbanDict)
            console.log("UrbanDict: loaded")
        }
    }

    Dict {
        id: dictdDict
        onQuery: {
            url = "https://dict.org/bin/Dict?Form=Dict2&Database=*&Query=" + text
        }

        Component.onCompleted: {
            qd.dictService.registerDict(dictdDict)
            console.log("DictdDict: loaded")
            url = "https://dict.org/bin/Dict?Form=Dict2&Database=*"
        }
        property url url
    }

    Dict {
        id: mockDict

        onQuery: {
            Data.sources.forEach(source => mockDict.queryResult(source))
        }

        Component.onCompleted: {
            qd.dictService.registerDict(mockDict)
            console.log("MockDict: loaded")
        }
    }

    SystemTrayIcon {
        id: systemTrayIcon
        visible: true
        icon.source: "qrc:/images/QuickDict-32x32.png"

        menu: Menu {
            visible: false
            MenuItem {
                text: qsTr("Show")
                onTriggered: {
                    showOnTop()
                    textField.forceActiveFocus()
                    textField.selectAll()
                }
            }
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }

        onActivated: {
            showOnTop()
            textField.forceActiveFocus()
            textField.selectAll()
        }
    }

    Timer {
        // use timer to reduce the number of save operations
        id: saveGeometryTimer
        interval: 500

        onTriggered: saveGeometry()
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

    Connections {
        target: qd.dictService
        function onQuery(text) {
            textField.text = text
            showOnTop()
        }
    }

    onXChanged: startSaveGeometryTimer()
    onYChanged: startSaveGeometryTimer()
    onWidthChanged: startSaveGeometryTimer()
    onHeightChanged: startSaveGeometryTimer()

    Component.onCompleted: {
        let geometry = qd.configCenter.value("geometry")
        if (typeof geometry !== "undefined") {
            window.x = geometry.x
            window.y = geometry.y
            window.width = geometry.width
            window.height = geometry.height
        }

        textField.forceActiveFocus()

        console.log(qd, qd.ocrEngine, qd.ocrEngine.isRunning())
        /* urbanDict.query("hack") */
    }

    function showOnTop() {
        window.show()
        window.raise()
        window.requestActivate()
    }

    function saveGeometry() {
        qd.configCenter.setValue("geometry", Qt.rect(window.x, window.y, window.width, window.height))
    }

    function startSaveGeometryTimer() {
        saveGeometryTimer.restart()
    }
}
