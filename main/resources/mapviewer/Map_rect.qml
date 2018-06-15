import QtQuick 2.0
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.4
//import episim.city 1.0


Rectangle{
    id: map_rect

    ToolBar {
        id: toolBar
        opacity: 1
        anchors.top: parent.top
        width: parent.width
        height: 25
        RowLayout{
            spacing: 10
            anchors.fill: parent

            Text {
                id: selected_pop_header
                text: "Selected pop: " + backend.selected_pop
                verticalAlignment: Text.AlignVCenter
                //Layout.fillWidth: true
                Layout.preferredWidth: parent.width / 5
            }

            Text {
                id: total_infected
                property int perc: backend.selected_pop === 0 ? 0 : Math.round(backend.selected_infected / backend.selected_pop * 100)
                text: "Selected infected: " + backend.selected_infected + "|"+ perc + "%"
                verticalAlignment: Text.AlignVCenter
                //Layout.fillWidth: true

            }

            Text{
               verticalAlignment: Text.AlignVCenter
               text: "Total pop: " + backend.total_pop

            }

            Text{
               verticalAlignment: Text.AlignVCenter
               property real perc: backend.total_pop === 0 ? 0 : Math.round(backend.total_infected / backend.total_pop * 100)
               text: "Total infected: " + backend.total_infected + "|" + perc + "%"
            }


            Button {
                Layout.fillHeight: true
                id: button
                text: qsTr("RUN PopBuilder")

                onClicked: {
                    map.clearMapItems();
                    backend.genPop();
                    map.center_and_zoom();

                }

                background: Rectangle {
                    id: pop_b
                    opacity: enabled ? 1 : 0.3
                    color: button.down ?  "red" : button.hovered ? "white" : "green"
                }
            }
            Button {
                id: button_sim
                text: qsTr("RUN sim")
                onClicked:{
                    if(backend.should_redraw()){
                        map.clearMapItems();
                    }

                    backend.run_simulator();
                }

                background: Rectangle {
                    id: run_sim
                    opacity: enabled ? 1 : 0.3
                    color: button_sim.down ?  "red" : button_sim.hovered ? "white" : "green"
                }

            }
        }
    }

     Rectangle{
         id: sidebar
         anchors.left: parent.left
         anchors.top: toolBar.bottom
         anchors .right: map.left
         anchors.bottom: map.bottom
         width: parent.width / 7

         ScrollView{
             clip: true
             anchors.fill: parent
        ColumnLayout{
            spacing: 5
            anchors.fill: parent
            RowLayout{
                Text{
                    text: "Selected Cities"
                    font.pointSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Repeater{
                model: backend.cities
               RowLayout{
                   enabled:modelData.clicked ? true : false
                   visible: modelData.clicked ? true: false
                   Layout.maximumHeight: modelData.clicked ? 55 : 0
                   Layout.minimumHeight: modelData.clicked ? 55 : 0
                   ColumnLayout{
                       Text{
                           enabled:modelData.clicked ? true : false
                           visible:modelData.clicked ? true : false
                           text: modelData.name
                       }

                       Text{
                           enabled:modelData.clicked ? true : false
                           visible:modelData.clicked ? true : false
                           text: "Population: " + modelData.popCount
                           font.pointSize: 10
                       }

                       Text{
                           enabled:modelData.clicked ? true : false
                           visible:modelData.clicked ? true : false
                           text: "Infected: " + modelData.infected+ "|" + Math.round(modelData.infected/modelData.popCount*100)+"%"
                           font.pointSize: 10
                       }

                   }
                }
            }
            Item{
                Layout.fillHeight:true
            }
        }
}
    }

    Map {
        id: map
        visible: true
        maximumTilt:0
        plugin: mapPlugin
        center: QtPositioning.coordinate(0, 0)
        zoomLevel: 0
        z:1

        anchors.top: toolBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: sidebar.right
        anchors.right: parent.right

        MapItemView{

            model: backend.cities
            delegate: mapCircleComponent

        }

        MapItemView{
            model: backend.commuters
            delegate: path
        }



        Rectangle {
                id: selectionRect
                visible: false
                x: 0
                y: 0
                z: backend.total_pop  +10
                width: 0
                height: 0
                rotation: 0
                color: "#5F227CEB"
                border.width: 1
                border.color: "#103A6E"
                transformOrigin: Item.TopLeft
                enabled: false
            }

        MouseArea{
            id: selectionMouseArea
            property int initialXPos
            property int initialYPos
            property bool justStarted
            z: 5
            anchors.fill: parent



            onPressed: {
                if (mouse.button === Qt.LeftButton && mouse.modifiers & Qt.ShiftModifier){
                    enabled = true
                    selectionRect.enabled = true
                    selectionRect.x = mouse.x
                    selectionRect.y = mouse.y
                    selectionRect.width = 0
                    selectionRect.height = 0
                    selectionRect.visible = true
                    map.enabled = false
                    justStarted = true
                }
            }
            onPositionChanged: {
                if(selectionRect.visible == true){

                    if (mouse.x < selectionRect.x)
                    {
                        if (mouse.y > selectionRect.y){ // from the right top corner
                            selectionRect.rotation = 90
                            selectionRect.width = mouse.y - selectionRect.y
                            selectionRect.height = selectionRect.x - mouse.x

                        }
                        else{
                            selectionRect.rotation = 180
                            selectionRect.width = selectionRect.x - mouse.x
                            selectionRect.height = selectionRect.y - mouse.y

                        }

                    }
                    else // x is bigger
                    {
                        if (mouse.y >= selectionRect.y){ // from the left top corner
                            selectionRect.rotation = 0
                            selectionRect.width  = mouse.x - selectionRect.x
                            selectionRect.height = mouse.y - selectionRect.y
                        }
                        else{
                            selectionRect.rotation = 270
                            selectionRect.width = selectionRect.y - mouse.y
                            selectionRect.height = mouse.x - selectionRect.x
                        }
                    }
                }

            }
            onReleased: {
                selectionRect.visible = false
                map.enabled = true
                //enabled = false

                var x_low
                var x_high
                var y_low
                var y_high

                if(selectionRect.rotation === 0){
                    x_low  = selectionRect.x
                    x_high = selectionRect.x + selectionRect.width
                    y_low  = selectionRect.y
                    y_high = selectionRect.y + selectionRect.height
                }

                else if(selectionRect.rotation === 90){
                    x_low  = selectionRect.x - selectionRect.height
                    x_high = selectionRect.x
                    y_low  = selectionRect.y
                    y_high = selectionRect.y + selectionRect.width

                }

                else if(selectionRect.rotation === 180){
                    x_low  = selectionRect.x - selectionRect.width
                    x_high = selectionRect.x
                    y_low  = selectionRect.y - selectionRect.height
                    y_high = selectionRect.y

                }

                else{
                    x_low  = selectionRect.x
                    x_high = selectionRect.x + selectionRect.height
                    y_low  = selectionRect.y - selectionRect.width
                    y_high = selectionRect.y
                }

                selectionRect.x = -1
                selectionRect.y = -1
                selectionRect.width = -1
                selectionRect.height = -1

                var items = []
                for(var i = 0; i < map.mapItems.length; i++){
                    var item = map.mapItems[i]
                    if(item.x > x_low && item.x < x_high){
                        if(item.y > y_low && item.y < y_high){
                            if(item.circle === true){
                                items.push(item.city)

                            }
                        }
                    }
                }
               backend.flip_items(items)
            }
        }


        function center_and_zoom(){
            map.zoomLevel = 7.5; // Fixed for now
            map.center = backend.center
        }
    }


    Component{
        id: path
        MapPolyline{
            line.width: modelData.width
            line.color: modelData.color
            path:[modelData.center_city1,modelData.center_city2 ]
        }

    }
    
    Component{
        id: mapCircleComponent
        MapCircle{
            property var city : model.modelData
            property var perc : city.popCount === 0 ? 0 : Math.round(city.infected/city.popCount * 100)
            property var commuting_lst: []
            property bool circle : true
            radius:  city.popCount/ backend.total_pop * 250000
            color: cty_mouse.containsMouse ? Qt.rgba(1, 1, 1, 0.2) : city.clicked ? Qt.rgba(1 - perc / 50 ,0,0 + perc /50,0.2 + perc / 50) : Qt.rgba(0,1 - perc / 50,0 + perc / 50 ,0.2 + perc / 50 )
            center: city.crd
            z: backend.total_pop - city.popCount
            
            MouseArea{
                id: cty_mouse
                anchors.fill: parent
                hoverEnabled: true
                ToolTip.text: "City: " + city.name + "\nPopulation: "
                              + city.popCount + "\nInfected: " + city.infected
                              + "|" + perc +"%"
                ToolTip.visible: containsMouse ? true : false
                
                onClicked: {
                    if(parent.city.clicked){
                        parent.city.clicked = false
                        //parent.remove_commuters();

                    }
                    else{
                        parent.city.clicked = true
                        //parent.draw_commuters()
                    }
                }
                
            }
        }
    }

}
