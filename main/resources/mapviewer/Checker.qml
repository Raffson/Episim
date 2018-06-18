import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

RowLayout{
    id: checkRandomAges
    Layout.alignment: Qt.AlignTop
    Layout.maximumHeight: 4
    property bool gg: geo
    Label{
        id: rndm_age_lbl
        verticalAlignment: Text.AlignVCenter
        Layout.minimumWidth: 245
        Layout.alignment: Qt.AlignLeft
        text: txt
    }
    
    CheckBox{
        id: rndm_age
        checked: backend.getBoolConfig(tagi, gg)
        onClicked:backend.setBoolConfig(tagi, checked, gg);
    }
}
