import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    width: 600
    height: 400

    title: qsTr("QuickDcit")

    Label {
        text: qsTr("You are on the Main page.")
        anchors.centerIn: parent
    }

     Rectangle {
     width: 48
     height: 48
     anchors.bottom: parent.bottom

     Image {
        anchors.fill: parent
        source: "https://www.baidu.com/favicon.ico"
     }
 }

}
