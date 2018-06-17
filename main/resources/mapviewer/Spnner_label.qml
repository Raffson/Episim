import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

RowLayout{
    id: spnner_label
    z:5
    spacing: 50
    Layout.alignment: Qt.AlignTop
    Layout.maximumHeight: 4
    
    property string txti: txt
    property string tg: tagi
    property real decis: decims
    property real stps: stpsize
    property int mini: mn
    property int maxi: mx
    
    Label{
        id: label
        text: parent.txti
        verticalAlignment: Text.AlignVCenter
        Layout.minimumWidth: 200
        Layout.alignment: Qt.AlignLeft
    }
    
    SpinBox {
        id: spinBox9
        font.pointSize: 10
        Layout.minimumWidth: 100
        Layout.alignment: Qt.AlignRight

        stepSize: stps
        decimals: decis
        minimumValue: mn
        maximumValue: mx === -1 ? Infinity : mx
        value:backend.getConfig(tg)
        onValueChanged: backend.setConfig(tg, value)
    }
    
}
