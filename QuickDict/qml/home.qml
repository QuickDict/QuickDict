import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1
import com.quickdict.components 1.0
import "data.js" as Data
import "axios.min.js" as Axios

ApplicationWindow {
    id: window
    title: stackView.depth > 1 ? stackView.currentItem.title + qsTr(" - QuickDcit") : qsTr("QuickDcit")
    width: dp(280)
    height: dp(400)
    minimumWidth: dp(250)
    minimumHeight: dp(200)
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
            placeholderText: qsTr("look up...")
            visible: stackView.depth <= 1
            anchors {
                left: toolButton.right
                right: parent.right
                margins: dp(20)
            }

            Monitor {
                id: textFieldMonitor
                name: qsTr("TextFieldMonitor")
                enabled: true
                description: qsTr("TextFieldMonitor monitors text in TextField.")

                Component.onCompleted: {
                    qd.registerMonitor(textFieldMonitor)
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
        width: Math.min(dp(256), 0.6 * window.width)
        height: window.height
        font.pixelSize: sp(14)

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
            ItemDelegate {
                text: qsTr("About")
                width: parent.width
                onClicked: {
                    stackView.push("About.ui.qml")
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
        name: qsTr("UrbanDict")
        enabled: true
        description: qsTr("UrbanDict uses data from https://www.urbandictionary.com.")
        property url url: "https://api.urbandictionary.com/v0/define?term="

        onQuery: {
            axios.get(url + text)
                .then(function (response) {
                    if (!response.data.list.length)
                        return

                    let result = {"engine": "Urban Dictionary", "text": response.data.list[0].word, "type": "lookup"}
                    let definitions = {"partOfSpeech": "", "list": []}
                    for (const entry of response.data.list) {
                        definitions.list.push({"definition": entry.definition, "examples": entry.example})
                    }
                    result.definitions = [definitions]
                    urbanDict.queryResult(result)
                })
                .catch(function (error) {
                    console.log("UrbanDict:", error)
                })
        }

        Component.onCompleted: {
            qd.registerDict(urbanDict)
        }
    }

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

    Dict {
        id: googleTranslate
        name: qsTr("Google Translate")
        enabled: true
        description: qsTr("DictdDict uses data from https://translate.google.com.")
        property url url: "https://translate.google.com/#view=home&op=translate&sl=auto&tl=en&text="

        onQuery: {
            let result = {"engine": name, "text": text, "type": "translation", "url": url + text}
            googleTranslate.queryResult(result)
        }

        Component.onCompleted: {
            qd.registerDict(googleTranslate)
        }
    }

    Dict {
        id: moeDict
        name: qsTr("MoeDict")
        enabled: true
        description: qsTr("MoeDict uses data from https://www.moedict.tw.")
        property url url: "https://www.moedict.tw/uni/"

        onQuery: {
            axios.get(url + text)
                .then(function (response) {
                    let data = response.data.heteronyms[0]
                    let result = {"engine": "萌典", "text": response.data.title, "type": "lookup"}
                    result.phonetic = [{"text": `/${data.bopomofo}/`}, {"text": `/${data.bopomofo2}/`}]
                    result.definitions = []
                    let definitions = {}
                    for (const entry of data.definitions) {
                        if (!(entry.type in definitions))
                            definitions[entry.type] = {"partOfSpeech": entry.type, "list": []}
                        let examples = entry.example ? entry.example : []
                        let quotes = entry.quote ? entry.quote : []
                        definitions[entry.type].list.push({"definition": entry.def, "examples": examples.concat(quotes)})
                    }
                    for (const e of Object.values(definitions)) {
                        result.definitions.push(e)
                    }
                    moeDict.queryResult(result)
                })
                .catch(function (error) {
                    console.log("MoeDict:", error)
                })
        }

        Component.onCompleted: {
            qd.registerDict(moeDict)
        }
    }

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

    Loader {
        id: userInitLoader
        source: StandardPaths.locate(StandardPaths.ConfigLocation, "QuickDict/qml/init.qml")
        active: false
        asynchronous: true
    }

    FontLoader {
        id: aliceInWonderlandFont
        source: "qrc:/fonts/Alice_in_Wonderland_3.ttf"
    }

    Timer {
        // use timer to reduce the number of save operations
        id: saveGeometryTimer
        interval: 500

        onTriggered: saveGeometry()
    }

    Connections {
        target: qd
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
        if (geometry) {
            window.x = geometry.x
            window.y = geometry.y
            window.width = geometry.width
            window.height = geometry.height
        }

        userInitLoader.active = true

        textField.forceActiveFocus()
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
