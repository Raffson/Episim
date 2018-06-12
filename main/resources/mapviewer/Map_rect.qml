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
                id: total_pop
                property int selected_pop: 0
                text: "Selected pop: " + selected_pop
                verticalAlignment: Text.AlignVCenter
                //Layout.fillWidth: true
                Layout.preferredWidth: parent.width / 5
            }

            Text {
                id: total_infected
                property int perc: total_pop.selected_pop === 0 ? 0 : Math.round(backend.selected_infected / total_pop.selected_pop * 100)
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
                model: CityModel
               RowLayout{
                   enabled:clicked ? true : false
                   visible: clicked ? true: false
                   Layout.maximumHeight: clicked ? 55 : 0
                   Layout.minimumHeight: clicked ? 55 : 0
                   ColumnLayout{
                       Text{
                           enabled:clicked ? true : false
                           visible:clicked ? true : false
                           text: name
                       }

                       Text{
                           enabled:clicked ? true : false
                           visible:clicked ? true : false
                           text: "Population: " + popCount
                           font.pointSize: 10
                       }

                       Text{
                           enabled:clicked ? true : false
                           visible:clicked ? true : false
                           text: "Infected: " + infected+ "|" + Math.round(infected/popCount*100)+"%"
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
            /*model:cty_model
            delegate: mapCircleComponent */

            model:CityModel
            delegate: mapCircleComponent

        }

        Rectangle {
                id: selectionRect
                visible: false
                x: 0
                y: 0
                z: backend.total_pop + 10
                width: 0
                height: 0
                rotation: 0
                color: "#5F227CEB"
                border.width: 1
                border.color: "#103A6E"
                transformOrigin: Item.TopLeft
            }

        MouseArea{
            id: selectionMouseArea
            property int initialXPos
            property int initialYPos
            property bool justStarted
            z: backend.total_pop + 11
            anchors.fill: parent

            onPressed: {
                if (mouse.button === Qt.LeftButton && mouse.modifiers & Qt.ShiftModifier){
                    selectionRect.x = mouse.x
                    selectionRect.y = mouse.y
                    selectionRect.width = 0
                    selectionRect.height = 0
                    selectionRect.visible = true
                    map.enabled = false
                }
            }

            onPositionChanged: {
                if(selectionRect.visible == true){
                    if (justStarted == true && (mouse.x != initialXPos || mouse.y != initialYPos))
                    {
                        if (mouse.x >= initialXPos)
                        {
                            if (mouse.y >= initialYPos)
                                selectionRect.rotation = 0
                            else
                                selectionRect.rotation = -90
                        }
                        else
                        {
                            if (mouse.y >= initialYPos)
                                selectionRect.rotation = 90
                            else
                                selectionRect.rotation = -180
                        }

                        justStarted = false
                        //console.log("Selection rotation: " + selectionRect.rotation)
                    }

                    if (selectionRect.rotation == 0 || selectionRect.rotation == -180)
                    {
                        selectionRect.width = Math.abs(mouse.x - selectionRect.x)
                        selectionRect.height = Math.abs(mouse.y - selectionRect.y)
                    }
                    else
                    {
                        selectionRect.width = Math.abs(mouse.y - selectionRect.y)
                        selectionRect.height = Math.abs(mouse.x - selectionRect.x)
                    }
                }

            }
            onReleased: {
                        selectionRect.visible = false
                        map.enabled = true
            }
        }

        function center_and_zoom(){
            map.zoomLevel = 7.5; // Fixed for now
            map.center = backend.center
        }
    }


    
    Component{
        id:path
        MapPolyline{
            property var cty
            property var cty2
            property int commuter_count_out: 0
            property int commuter_count_in: 0
            property bool out: false
            property bool inc: false
            line.width: calc_line_width() * 50
            line.color:mouse_line.containsMouse ? Qt.rgba(1,1,1,0.4) : (out && inc) ? 'blue' : (inc ? Qt.rgba(1,0,0,0.4): Qt.rgba(0,1,0,0.4))
            z: 1000000 - calc_line_width()
            
            
            
            function calc_line_width(){
                
                if(out && inc){
                    return (commuter_count_in + commuter_count_out) / (cty.total_out_commuters + cty.total_in_commuters)
                }
                
                else if(inc){
                    return commuter_count_in / cty.total_in_commuters
                }
                
                else{
                    
                    return commuter_count_out / cty.total_out_commuters
                }
            }
            
            function generate_tool_text(){
                var ret = cty.name
                
                if(out && inc){
                    ret += " <-> " + cty2.name + "\n"
                }
                
                else if(out){
                    ret += " -> " + cty2.name + "\n"
                }
                
                else{
                    ret += " <- " + cty2.name + "\n"
                }
                
                if (out){
                    ret = ret + "Outgoing commuters: " + commuter_count_out
                }
                
                if(out && inc){
                    ret += "\n"
                }
                
                if(inc){
                    ret = ret + "Incomming commuters: " + commuter_count_in
                }
                
                return ret
            }
            
            MouseArea{
                id: mouse_line
                anchors.fill: parent
                hoverEnabled: true
                ToolTip.visible: containsMouse ? true : false
                ToolTip.text: generate_tool_text()
            }
        }
    }
    
    Component{
        id: mapCircleComponent
        MapCircle{
            function draw_commuters(){
                
                // Outs first to filter duplicates
                for(var i = 0; i < 10; i++){
                    var added = false
                    for(var k = 0; k < 10; k++){
                        if(city.out_commuters[i] === city.in_commuters[k]){
                            var pat_b = path.createObject(map,{
                                                              cty: city ,
                                                              cty2: backend.get_city(city.out_commuters[i]),
                                                              commuter_count_out: city.out_commuters_count[i],
                                                              commuter_count_in: city.in_commuters_count[k],
                                                              inc: true,
                                                              out: true
                                                          });
                            pat_b.addCoordinate(city.crd)
                            pat_b.addCoordinate(backend.get_city(city.out_commuters[i]).crd)
                            
                            map.addMapItem(pat_b)
                            commuting_lst.push(pat_b)
                            added = true
                            break
                        }
                    }
                    if(!added){
                        pat_b = path.createObject(map, {cty: city , cty2: backend.get_city(city.out_commuters[i]), commuter_count_out: city.out_commuters_count[i], out: true});
                        pat_b.addCoordinate(city.crd)
                        pat_b.addCoordinate(backend.get_city(city.out_commuters[i]).crd)
                        map.addMapItem(pat_b)
                        commuting_lst.push(pat_b)
                    }
                }
                
                for(i = 0; i < 10; i++){
                    for(k = 0; k <10; k++){
                        added = false
                        if(city.in_commuters[i] === city.out_commuters[k]){
                            added = true
                            break
                        }
                    }
                    if(!added){
                        pat_b = path.createObject(map, {cty: city ,cty2: backend.get_city(city.in_commuters[i]), commuter_count_in: city.in_commuters_count[i], inc: true});
                        pat_b.addCoordinate(city.crd)
                        pat_b.addCoordinate(backend.get_city(city.in_commuters[i]).crd)
                        map.addMapItem(pat_b)
                        commuting_lst.push(pat_b)
                    }
                }
            }
            
            function remove_commuters(){
                for(var i = 0; i < commuting_lst.length; i++){
                    map.removeMapItem(commuting_lst[i]);
                    
                }
                commuting_lst = []
            }
            
            property var city : model.modelData
            property var perc : city.popCount === 0 ? 0 : Math.round(city.infected/city.popCount * 100)
            property var commuting_lst: []
            radius: (city.popCount / backend.total_pop) * 250000
            color: (cty_mouse.containsMouse || clicked) ? Qt.rgba(1 - perc / 50 ,0,0 + perc /50,0.2 + perc / 50) : Qt.rgba(0,1 - perc / 50,0 + perc / 50 ,0.2 + perc / 50 )
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
                        total_pop.selected_pop -= parent.city.popCount
                        parent.remove_commuters();

                    }
                    else{
                        parent.city.clicked = true
                        total_pop.selected_pop += parent.city.popCount
                        parent.draw_commuters()
                    }
                }
                
            }
        }
    }

}
