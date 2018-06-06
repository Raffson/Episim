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
            property var clr
            line.width: 2
            line.color: clr

        }
    }

    Component{
        id: mapCircleComponent
        MapCircle{
            function draw_commuters(){
                for(var i = 0; i < 10; i++){

                    var pat = path.createObject(map, {clr: 'green'});
                    pat.addCoordinate(city.crd)
                    pat.addCoordinate(backend.get_city(city.out_commuters[i]).crd)

                    console.log(backend.get_city(city.in_commuters[i]))
                    var path2 = path.createObject(map, {clr: 'red'});
                    path2.addCoordinate(city.crd)
                    path2.addCoordinate(backend.get_city(city.in_commuters[i]).crd)

                    map.addMapItem(pat);
                    map.addMapItem((path2));

                    commuting_lst.push(pat);
                    commuting_lst.push(path2);
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
            z: backend.total_pop - city.popCount

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
