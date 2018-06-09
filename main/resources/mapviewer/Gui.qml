import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.11
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.0

ApplicationWindow{
    id: window
    width: 1200
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

            Tab{
                id: config_pop
                active: false
                title:"config pop"
                Rectangle{
                    anchors.fill: parent
                    color: Qt.rgba(0.9,0.9,0.9,0.5)
                    GridLayout{
                        id: alligner

                        anchors.topMargin: parent.height / 50
                        anchors.leftMargin: 50
                        anchors.fill: parent
                        columnSpacing: 5
                        rowSpacing: 1
                        rows: 5
                        columns: 1
                        Repeater{
                            model: ["Population", "Fractions Students", "Fraction commuting students", "Fraction active workers"
                                ,"Fraction commuting workers",
                                "Average size", "College Size",
                                "Community size",
                                "Workplace Size"]
                            RowLayout{
                                z:5
                                spacing: 50
                                Layout.alignment: Qt.AlignTop
                                Layout.maximumHeight: 10
                                Label{
                                    id: label
                                    text: modelData
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignVCenter
                                    Layout.minimumWidth: 200
                                    Layout.alignment: Qt.AlignLeft
                                }

                                SpinBox {
                                    id: spinBox9
                                    font.pointSize: 10
                                    Layout.minimumWidth: 100
                                    stepSize: 0
                                    //Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignRight
                                }

                            }
                        }
                        RowLayout{

                            TextField{

                            }

                            Button{
                                text: "..."
                                onClicked: cty.visible = true
                            }

                        }



                        FileDialog{
                            id: cty
                            title: "Please choose a file"
                            folder: shortcuts.home
                               onAccepted: {
                                   console.log("You chose: " + fileDialog.fileUrls)

                               }
                               onRejected: {
                                   console.log("Canceled")

                               }


                        }
                    }
                }
            }

        }
    }
}


/*##^## Designer {
    D{i:5;invisible:true}D{i:8;anchors_x:5}D{i:4;invisible:true}
}
 ##^##*/
