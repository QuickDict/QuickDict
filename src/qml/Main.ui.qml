import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebView 1.15

Page {
    title: qsTr("QuickDcit")

    WebView {
        id: webView
        anchors.fill: parent
        url: dictdDict.url
    }

    Connections {
        target: qd.dictService
        function onQueryResult(result) {
        }
    }
}
