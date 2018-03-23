import QtLocation 5.3
import QtQuick 2.7

MapCircle {
    id: cityCircle
    property variant population:0
    property alias area_text: m_area.info_text
    MouseArea {
        id: m_area
        property alias info_text: info.text
        Text {
            id: info
            text: "gfeqgefqvzeg"
            color: "blue"
            font.pointSize: 1
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom
            anchors.topMargin: 5
        }
        anchors.fill:parent
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
        onEntered:{
            info.font.pointSize = 16;
            parent.color = "red";
            parent.opacity = 1;
        }
        onExited:{
            info.font.pointSize = 1;
            parent.color = "green";
            parent.opacity = 0.25;
        }
        onClicked:{
            info.font.pointSize = 16;
            parent.color = "red";
            parent.opacity = 1;
        }
    }
}