import QtQuick 2.15
import com.quickdict.components 1.0
import "UrbanDict" as UrbanDict
import "DictdDict" as DictdDict
import "GoogleTranslate" as GoogleTranslate
import "MoeDict" as MoeDict
import "MockDict" as MockDict

Item {
    UrbanDict.UrbanDict {}
    DictdDict.DictdDict {}
    GoogleTranslate.GoogleTranslate {}
    MoeDict.MoeDict {}
    MockDict.MockDict {}
}
