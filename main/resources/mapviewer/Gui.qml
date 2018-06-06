import QtQuick 2.2
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.4

import episim.backend 1.0
import episim.city 1.0


ApplicationWindow{
    id: window
    width: 900
    height: 500
    title: "Simulator"
    visible: true
    //visibility: "FullScreen"


    Plugin {
        id: mapPlugin
        name:  "mapbox"//", "esri", ...
        PluginParameter{ name: "mapbox.access_token";
            value: "pk.eyJ1Ijoicm9iYmVoZWlybWFuIiwiYSI6ImNqaTBhYWY2bjEyZG8zcHBncmN5amc4ajUifQ.IiFHtjyHqO0Mrl5Xz_5aug"
        }


    }

    Rectangle{
        id: data_viewer
        width: 200
        height: 100

        anchors.left: parent.left
        anchors.leftMargin: parent.width /15

        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height / 15
        color: Qt.rgba(0.9,0.9,0.9,1)
        z: 1

        Text {

            id: total_pop
            property int selected_pop: 0
            text: "Selected pop: " + selected_pop
        }
    }

    BackEnd{
        id: backend
    }

    ToolBar {
        id: toolBar
        opacity: 1
        anchors.top: parent.top
        width: parent.width
        height: 25

        Button {
            id: button
            text: qsTr("RUN")
            width: parent.width / 10
            height: parent.height

            MouseArea {
                id: mouseArea_run
                anchors.fill: parent
                onClicked: {
                             map.clearMapItems();
                             backend.genPop()
                             map.draw_cities()
                             map.center_and_zoom()
                }

            }
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
            line.width: calc_line_width() * 100
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

            property City city
            property bool clicked: false
            property var commuting_lst: []
            radius: (city.popCount / backend.total_pop) * 250000
            color: (cty_mouse.containsMouse || clicked) ? Qt.rgba(1,0,0,0.2) : Qt.rgba(0,1,0,0.2)
            center: city.crd
            z: 1

            MouseArea{
                id: cty_mouse
                anchors.fill: parent
                hoverEnabled: true
                ToolTip.text: "City: " + city.name + "\nPopulation: " + city.popCount
                ToolTip.visible: containsMouse ? true : false

                onClicked: {
                    if(parent.clicked){
                        parent.clicked = false
                        total_pop.selected_pop -= parent.city.popCount
                        parent.remove_commuters();

                    }
                    else{
                        parent.clicked = true
                        total_pop.selected_pop += parent.city.popCount
                        parent.draw_commuters()
                    }
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

        anchors.top: toolBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right


        function draw_cities(){
                for(var i = 0; i < backend.cities.length; i++){
                    var circle = mapCircleComponent.createObject(map, {city: backend.cities[i]})
                    map.addMapItem(circle);
                }
        }

        function center_and_zoom(){
            map.zoomLevel = 7.5; // Fixed for now
            map.center = backend.center
        }
    }
}
