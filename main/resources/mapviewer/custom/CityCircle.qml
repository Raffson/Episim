import QtLocation 5.3
import QtQuick 2.7

MapCircle {
    id: cityCircle
    property variant count: 0
    property variant population:0
    property variant is_clicked: false
    property alias area_text: m_area.info_text

    function updateSelectedPopulation(counter){
        if (is_clicked == true){
            new_count = counter + population;
            return new_count;
        }
        else{
            return count;
        }
    }

    MouseArea {
        id: m_area
        property alias info_text: info.text
        Text {
            id: info
            text: ""
            color: "blue"
            font.pointSize: 1
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom
            anchors.topMargin: 5
        }
        anchors.fill:parent
        hoverEnabled: true
        enabled: true
        acceptedButtons: Qt.LeftButton
        onEntered:{
            info.font.pointSize = 16;
            parent.color = "red";
            parent.opacity = 1;
        }
        onExited:{
            if (is_clicked == false){
                info.font.pointSize = 1;
                parent.color = "green";
                parent.opacity = 0.25;
            }
        }
        onClicked:{
            if (parent.is_clicked == false){
                parent.is_clicked = true;
            }
            else{
                parent.is_clicked = false;
            }
        }
    }
}