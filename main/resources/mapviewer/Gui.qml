import QtQuick 2.2
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import episim.backend 1.0


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
                onClicked: { backend.genPop()
                             map.draw_cities()
                }

            }

            style: ButtonStyle {
                background: Rectangle {
                    color: mouseArea_run.pressed ? "#336699" : "#0099FF";
                    radius: 1;
                }
            }
        }
    }

    ListModel{
        id:mapModel
    }

    Map {
        id: map
        visible: true
        maximumTilt:0
        plugin: mapPlugin
        center: QtPositioning.coordinate(0, 0) // Oslo
        zoomLevel: 0

        anchors.top: toolBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        MapItemView{
            model:mapModel
            delegate: MapCircle{
                radius: 5000
                color: 'red'
                visible: true
                border.width: 3
                center{
                    latitude: lat
                    longitude: longi
                }
            }
        }

        function draw_cities(){
            for(var i = 0; i < backend.cities.length; i++){
                var cty = backend.cities[i]
                console.log(cty.crd.x)
                mapModel.append({lat : cty.crd.x , longi: cty.crd.y});

            }
        }
    }
}
