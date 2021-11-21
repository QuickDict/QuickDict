import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: aboutPage
    title: qsTr("About")
    background: null

    ScrollView {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        contentWidth: parent.width
        contentHeight: layout.implicitHeight + dp(16) // with margins
        clip: true

        ColumnLayout {
            id: layout
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: dp(8)
                topMargin: dp(20)
            }
            spacing: dp(16)

            Text {
                text: "QuickDict"
                font.bold: true
                font.italic: true
                font.pixelSize: sp(60)
                font.family: aliceInWonderlandFont.name
                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    ColumnLayout {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: dp(8)
        }

        TextEdit {
            text: qsTr('<a href="https://github.com/QuickDict/QuickDict">QuickDict</a> is licensed under the <a href="https://www.gnu.org/licenses/gpl-3.0.html">GPLv3</a> license.')
            textFormat: Text.RichText
            font.pixelSize: sp(12)
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            readOnly: true
            selectByMouse: true

            MouseArea {
                anchors.fill: parent
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor

                onClicked: Qt.openUrlExternally(parent.hoveredLink)
            }
        }
        Text {
            text: "Copyright (C) 2021 Zhiping Xu <2h1p1n9.xu@gmail.com>"
            font.pixelSize: sp(12)
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        Text {
            text: qsTr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE")
            font.pixelSize: sp(10)
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}

