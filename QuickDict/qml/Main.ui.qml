import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: mainPage
    objectName: "mainPage"
    title: qsTr("QuickDcit")
    background: null

    property alias currentIndex: view.currentIndex
    property var lookupResults: []
    property var translationResults: []

    SwipeView {
        id: view
        anchors.fill: parent

        Loader {
            id: lookupLoader
            source: "Lookup.ui.qml"
            asynchronous: true
            active: true

            onLoaded: {
                item.updateResults(lookupResults)
            }

            Connections {
                target: mainPage

                function onLookupResultsChanged() {
                    if (lookupLoader.item)
                        lookupLoader.item.updateResults(lookupResults)
                }
            }
        }
        Loader {
            id: translationLoader
            source: "Translation.ui.qml"
            asynchronous: true
            active: view.currentIndex == SwipeView.index

            onLoaded: {
                item.updateResults(translationResults)
            }

            Connections {
                target: mainPage

                function onTranslationResultsChanged() {
                    if (translationLoader.item)
                        translationLoader.item.updateResults(translationResults)
                }
            }
        }
    }

    PageIndicator {
        id: indicator

        count: view.count
        currentIndex: view.currentIndex

        anchors.bottom: view.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Connections {
        target: qd

        function onQuery(text) {
            lookupResults = []
            translationResults = []
            lookupResultsChanged()
            translationResultsChanged()
        }
        function onQueryResult(result) {
            if (result.type === "lookup") {
                lookupResults.push(result)
                lookupResultsChanged()
            }
            else if (result.type === "translation") {
                translationResults.push(result)
                translationResultsChanged()
            }
            else {
                console.log(`${result.engine} error: unknown query result of type ${result.type}`)
            }
        }
    }
}
