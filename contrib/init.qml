import QtQuick 2.15
import com.quickdict.components 1.0
import "UrbanDict" as UrbanDict
import "DictdDict" as DictdDict
import "GoogleTranslate" as GoogleTranslate
import "MoeDict" as MoeDict
import "MockDict" as MockDict

Item {
    UrbanDict.UrbanDict {
        id: urbanDict
        enabled: true
    }
    DictdDict.DictdDict {
        id: dictdDict
        enabled: true
    }
    GoogleTranslate.GoogleTranslate {
        id: googleTranslate
        enabled: true
    }
    MoeDict.MoeDict {
        id: moeDict
        enabled: true
    }
    MockDict.MockDict {
        id: mockDict
        enabled: false
    }

    Shortcut {
        // hide QuickDict window
        sequence: "Esc"
        context: Qt.ApplicationShortcut
        onActivated: window.hide()
    }
    Shortcut {
        // quit QuickDict
        sequence: StandardKey.Quit
        context: Qt.ApplicationShortcut
        onActivated: Qt.quit()
    }
    Shortcut {
        // focus on TextField
        sequence: "Alt+L"
        context: Qt.ApplicationShortcut
        onActivated: {
            while (stackView.depth > 1)
                stackView.pop()
            if (drawer.activeFocus)
                drawer.close()
            textField.forceActiveFocus()
            textField.selectAll()
        }
    }
    Shortcut {
        // scroll down lookup page
        sequence: "J"
        enabled: mainPage.currentIndex == 0
        property var mainPage: {"currentIndex": 0} // this default value is to prevent warning of undefined property
        property var lookupPage

        onActivated: {
            if (mainPage.currentIndex == 0)
                lookupPage.scrollUp()
        }
        Component.onCompleted: {
            // wait all components are loaded
            setTimeout(() => {
                mainPage = qd.findChild("mainPage", window)
                lookupPage = qd.findChild("lookupPage", window)
            }, 500)
        }
    }
    Shortcut {
        // scroll up lookup page
        sequence: "K"
        enabled: mainPage.currentIndex == 0
        property var mainPage: {"currentIndex": 0} // this default value is to prevent warning of undefined property
        property var lookupPage
        onActivated: {
            if (mainPage.currentIndex == 0)
                lookupPage.scrollDown()
        }
        Component.onCompleted: {
            // wait all components are loaded
            setTimeout(() => {
                mainPage = qd.findChild("mainPage", window)
                lookupPage = qd.findChild("lookupPage", window)
            }, 500)
        }
    }
    Hotkey {
        // activate QuickDict
        sequence: "Alt+Q"
        onActivated: {
            if (!window.visible) {
                window.show()
                textField.forceActiveFocus()
                textField.selectAll()
            }
            window.raise()
            window.requestActivate()
        }
    }
    Hotkey {
        // toogle ClipboardMonitor
        sequence: "Alt+C"
        onActivated: {
            let m = qd.monitor("ClipboardMonitor")
            if (m)
                m.toggle()
        }
    }
    Hotkey {
        // toogle MouseOverMonitor
        sequence: "Alt+O"
        onActivated: {
            let m = qd.monitor("MouseOverMonitor")
            if (m)
                m.toggle()
        }
    }
}
