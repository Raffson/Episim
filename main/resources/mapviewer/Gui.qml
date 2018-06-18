import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

ApplicationWindow{
    id: window
    width: 1400
    height: 800
    title: "Simulator"
    visible: true
    //visibility: "FullScreen"



    Component{
        id: lbl
        Label{
            id: label
            lineHeight: 0.9
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }


    }

    Rectangle{
        TabView{
            width: window.width
            height: window.height
            currentIndex: 1
            Tab{
                id: map_tab
                title: "Map"
                Map_rect {
                    id: map_rect
                }
            }

            Config_pop {
                id: config_pop
                active: false
                title: "config pop"
            }

            Config_sim {
                id: config_sim
            }
        }
    }
}


/*##^## Designer {
    D{i:5;invisible:true}D{i:8;anchors_x:5}D{i:4;invisible:true}
}
 ##^##*/
