import QtQuick 2.0
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.4
//import episim.city 1.0


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
        TabView{
            width: window.width
            height: window.height
            Tab{
                title: "Map"
                Map_rect {
                    id: map_rect
                }
            }

            Tab{
                id: config_pop
                title:"config pop"

                ColumnLayout {
                    id: columnLayout
                    width: 795
                    height: 444

                    RowLayout {
                        id: rowLayout
                        width: 100
                        height: 100
                        Layout.preferredHeight: 0
                        Layout.preferredWidth: 0
                        Layout.fillWidth: true
                        visible: true

                        Text {
                            id: text1
                            y: 0
                            width: 500
                            height: 50
                            color: "#555753"
                            text: qsTr("Text")
                            visible: true
                            font.pixelSize: 24
                        }
                    }
                }
            }

        }
    }
}
