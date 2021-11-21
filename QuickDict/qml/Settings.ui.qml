import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: settingsPage
    background: null
    title: qsTr("Settings")

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

            ColumnLayout {
                RowLayout {
                    spacing: dp(8)
                    Text {
                        text: qsTr("Monitors")
                        font.bold: true
                        font.italic: true
                        font.pixelSize: sp(20)
                        color: Qt.rgba(0, 0, 0, 0.6)
                    }
                    Rectangle {
                        Layout.preferredHeight: 2
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        color: Qt.rgba(0, 0, 0, 0.38)
                    }
                }

                Repeater {
                    model: qd.monitors.sort((first, second) => first.name.localeCompare(second.name))
                    delegate: CheckBox {
                        checked: modelData.enabled
                        text: modelData.name

                        onToggled: {
                            modelData.toggle()
                        }
                        Component.onCompleted: {
                            if (text === "TextFieldMonitor")
                                enabled = false
                        }
                    }
                }
            }

            ColumnLayout {
                RowLayout {
                    spacing: dp(8)
                    Text {
                        text: qsTr("Dictionaries")
                        font.bold: true
                        font.italic: true
                        font.pixelSize: sp(20)
                        color: Qt.rgba(0, 0, 0, 0.6)
                    }
                    Rectangle {
                        Layout.preferredHeight: 2
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        color: Qt.rgba(0, 0, 0, 0.38)
                    }
                }

                Repeater {
                    model: qd.dicts.sort((first, second) => first.name.localeCompare(second.name))
                    delegate: CheckBox {
                        checked: modelData.enabled
                        text: modelData.name

                        onToggled: modelData.toggle()
                    }
                }
            }
        }
    }
}
