import QtQuick 2.0
import QtQuick.Controls 2.2
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1


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


    ToolBar {
        id: toolBar
        opacity: 1
        anchors.top: parent.top
        width: parent.width
        height: 25
        clip: true

        Button {
            id: button
            text: qsTr("RUN")
            width: parent.width / 10
            height: parent.height

        }
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

    }
}
