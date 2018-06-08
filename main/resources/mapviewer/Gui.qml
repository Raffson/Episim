import QtQuick 2.1
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
                id: map_tab
                title: "Map"
                Map_rect {
                    id: map_rect
                }
            }

            Tab{
                id: config_pop
                title:"config pop"

                Grid {
                    id: grid
                    anchors.leftMargin: parent.width / 20
                    anchors.topMargin: parent.height / 20
                    spacing: 25
                    anchors.fill: parent
                    rows: 5
                    columns: 2

                    Row {
                        id: row
                        spacing: 5
                        layoutDirection: Qt.RightToLeft

                        TextField {
                            id: textField
                            placeholderText: qsTr("Text Field")
                        }

                        Label {
                            id: label
                            text: qsTr("Population:")
                            topPadding: textField.height / 4
                            lineHeight: 0.9
                            verticalAlignment: Text.AlignTop
                        }

                    }
                }

            }
        }
    }
}
