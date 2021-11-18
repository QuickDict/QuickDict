import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: settingsPage
    background: null
    title: qsTr("Settings")

    Label {
        text: qsTr("You are on Settings page.")
        anchors.centerIn: parent
    }

    TextEdit {
        id: text
        textFormat: Text.RichText
        text: "See the <a href=\"http://qt-project.org\">Qt Project website</a>."
        font.pixelSize: sp(20)
        readOnly: true
        selectByMouse: true

        onLinkActivated: console.log(link + " link activated")
        onLinkHovered: console.log(link + " link hovered")
    }
}
