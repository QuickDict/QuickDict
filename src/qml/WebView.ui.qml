import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebEngine 1.10

Page {
    title: qsTr("WebView")

    WebEngineView {
        id: webEngineView
        anchors.fill: parent
        url: dictdDict.url

        onLoadingChanged: {
            if (loadRequest.status == WebEngineLoadRequest.LoadSucceededStatus) {
                webEngineView.runJavaScript("window.scroll(0, 360)")
            }
        }
    }

    Connections {
        target: qd.dictService
        function onQueryResult(result) {
        }
    }
}
