import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebEngine 1.10

Page {
    id: translationPage
    title: qsTr("Translation")
    background: null

    WebEngineView {
        id: translationWebView
        anchors.fill: parent
    }

    function updateResults(results) {
        if (results.length) {
            // FIXME: we only show the first result for now
            translationWebView.url = results[0].url
        }
    }
}
