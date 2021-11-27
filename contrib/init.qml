import QtQuick 2.15
import com.quickdict.components 1.0
import "UrbanDict" as UrbanDict
import "DictdDict" as DictdDict
import "GoogleTranslate" as GoogleTranslate
import "MoeDict" as MoeDict
import "MockDict" as MockDict

Item {
    UrbanDict.UrbanDict { id: urbanDict }
    DictdDict.DictdDict { id: dictdDict}
    GoogleTranslate.GoogleTranslate { id: googleTranslate }
    MoeDict.MoeDict { id: moeDict }
    MockDict.MockDict { id: mockDict }

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
    Hotkey {
        // activate QuickDict
        sequence: "Alt+Q"
        onActivated: {
            if (!window.visible)
                window.show()
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
