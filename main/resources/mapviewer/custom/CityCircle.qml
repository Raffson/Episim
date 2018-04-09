import QtLocation 5.3
import QtQuick 2.7


Rectangle {
        id: cityCircle
        objectName: "city"
        property variant city_id: 0
        property variant count: 0
        property variant population:0
        property variant is_clicked: false
        property variant in_commuting: 0
        property variant out_commuting: 0
        property alias area_text: m_area.info_text

        color: 'green'
        border.width: 3

        function isSelected(){
            if (is_clicked == true){
                return population;
            }
            else{
                return 0;
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
