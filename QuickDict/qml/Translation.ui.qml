import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebEngine 1.10
import QtQuick.Layouts 1.15

Page {
    id: translationPage
    objectName: "translationPage"
    title: qsTr("Translation")
    background: null

    ColumnLayout {
        anchors.fill: parent

        WebEngineView {
            id: translationWebView
            Layout.fillWidth: true
            Layout.fillHeight: true
            width: 200
            height: 300
        }
        RowLayout {
            visible: translationButtonRepeater.model.length > 1
            Repeater {
                id: translationButtonRepeater

                Button {
                    icon.source: getBaseUrl(modelData.url) + "/favicon.ico"
                    icon.color: "transparent"
                    onClicked: translationWebView.url = modelData.url
                }
            }
        }
    }

    function updateResults(results) {
        if (results.length)
            translationWebView.url = results[0].url
        else
            translationWebView.url = ""
        translationButtonRepeater.model = results
    }
    function getBaseUrl(url) {
        let re = RegExp("^.+?[^\/:](?=[?\/]|$)")
        return re.exec(url)
    }
}
