import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: mainPage
    background: null
    title: qsTr("QuickDcit")

    property var sources

    ScrollView {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: layout.implicitHeight + dp(16) // with margins
        clip: true

        ColumnLayout {
            id: layout
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: dp(8)
            }
            spacing: dp(16)

            Repeater {
                id: repeater

                ColumnLayout {
                    spacing: dp(8)

                    RowLayout {
                        spacing: dp(8)
                        Text {
                            text: modelData.engine
                            font.bold: true
                            font.italic: true
                            font.pixelSize: sp(20)
                            font.family: aliceInWonderlandFont.name
                            color: Qt.rgba(0, 0, 0, 0.6)
                        }
                        Rectangle {
                            Layout.preferredHeight: 2
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignCenter
                            color: Qt.rgba(0, 0, 0, 0.38)
                        }
                    }
                    RowLayout {
                        spacing: dp(8)
                        Text {
                            text: modelData.text
                            font.bold: true
                            font.pixelSize: sp(14)
                        }
                        Repeater {
                            model: modelData.phonetic
                            Text {
                                text: modelData.text
                                font.pixelSize: sp(12)
                                color: Qt.rgba(0, 0, 0, 0.87)
                            }
                        }
                    }
                    Repeater {
                        model: modelData.definitions
                        ColumnLayout {
                            Text {
                                text: modelData.partOfSpeech
                                visible: modelData.partOfSpeech
                                font.bold: true
                                font.pixelSize: sp(12)
                            }
                            ColumnLayout {
                                Layout.leftMargin: dp(8)

                                Repeater {
                                    model: modelData.list

                                    RowLayout {
                                        Text {
                                            text: index + 1
                                            font.pixelSize: sp(12)
                                            color: Qt.rgba(0, 0, 0, 0.6)
                                            Layout.alignment: Qt.AlignTop
                                        }
                                        ColumnLayout {
                                            TextEdit {
                                                Layout.fillWidth: true
                                                textFormat: Text.RichText
                                                text: modelData.definition
                                                font.pixelSize: sp(12)
                                                wrapMode: Text.WordWrap
                                                readOnly: true
                                                selectByMouse: true
                                            }
                                            Repeater {
                                                model: modelData.examples

                                                RowLayout {
                                                    Text {
                                                        text: "\u2022"
                                                        font.pixelSize: sp(12)
                                                        color: Qt.rgba(0, 0, 0, 0.6)
                                                        Layout.alignment: Qt.AlignTop
                                                    }
                                                    TextEdit {
                                                        Layout.fillWidth: true
                                                        textFormat: Text.RichText
                                                        text: modelData
                                                        font.pixelSize: sp(12)
                                                        wrapMode: Text.WordWrap
                                                        readOnly: true
                                                        selectByMouse: true
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: qd.dictService

        function onQuery(text) {
            sources = []
            repeater.model = sources
        }
        function onQueryResult(result) {
            sources.push(result)
            repeater.model = sources
        }
    }
}
