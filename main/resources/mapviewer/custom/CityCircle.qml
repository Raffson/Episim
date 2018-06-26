import QtLocation 5.3
import QtQuick 2.7
import QtQml 2.2

// Represents a city on the map.

Rectangle {
    id: cityCircle
    objectName: "city"
    property variant city_id: 0
    property variant count: 0
    property variant population:0
    property variant infected: 0
    property variant is_clicked: false
    property variant in_commuting: 0
    property variant in_commuting_size: 0
    property variant out_commuting: 0
    property variant out_commuting_size: 0
    property variant longi: 0
    property variant lati: 0
    property variant xco: 0
    property variant yco: 0
    property alias area_text: m_area.info_text
    property alias area_info: m_area.city_info

    color: Qt.hsva(0, 1, 0.5, 0.5)
    border.width: 3

    // Returns wether the city is selected or not.
    function isSelected(){
        var all_pop_info = {
            popcount: 0,
            infected: 0
        }
        if (is_clicked == true){
            all_pop_info.popcount = population
            all_pop_info.infected = infected
        }
        return all_pop_info;
    }

    // Used for interaction purposes.
    MouseArea {
        id: m_area
        property alias info_text: info.text
        property alias city_info: info
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

        // Behaviors.
        onEntered:{
            info.font.pointSize = 12;
            parent.opacity = 1;
        }
        onExited:{
            if (is_clicked == false){
                info.font.pointSize = 1;
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
