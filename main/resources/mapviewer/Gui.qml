import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

ApplicationWindow{
    id: window
    width: 1200
    height: 500
    title: "Simulator"
    visible: true
    //visibility: "FullScreen"

    Plugin {
        id: mapPlugin
        name: "osm" //"mapbox"//", "esri", ...
        /*PluginParameter{ name: "mapbox.access_token";
            value: "pk.eyJ1Ijoicm9iYmVoZWlybWFuIiwiYSI6ImNqaTBhYWY2bjEyZG8zcHBncmN5amc4ajUifQ.IiFHtjyHqO0Mrl5Xz_5aug"*/
       // }


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
                        flow: GridLayout.LeftToRight

                        anchors.topMargin: parent.height / 50
                        anchors.leftMargin: 50
                        anchors.fill: parent
                        columnSpacing: 5
                        rowSpacing: 1
                        rows: 10
                        columns: 1
                        ListModel{
                            id: num_bttns
                            ListElement { tag: "Population"; xml: "pop_info.pop_total"; fract: false}
                            ListElement { tag: "Fraction students"; xml: "pop_info.fraction_students"; fract: true}
                            ListElement { tag: "Fraction active workers"; xml: "pop_info.fraction_active_workers"; fract: true}
                            ListElement { tag: "Fraction commuting students"; xml: "pop_info.fraction_commuting_workers"; fract: true}
                            ListElement { tag: "Fraction commuting workers"; xml: "pop_info.fraction_commuting_workers"; fract: true}
                            ListElement { tag: "Average size"; xml:"contactpool_info.average_size"; fract: false }
                            ListElement { tag: "School Size";  xml:"contactpool_info.school.size";fract: false }
                            ListElement { tag: "College Size"; xml:"contactpool_info.college.size";fract: false }
                            ListElement { tag: "Workplace Size";  xml:"contactpool_info.workplace.size";fract: false }
                            ListElement { tag: "Nr cities with colleges"; xml:"contactpool_info.college.cities"; fract: false}
                            ListElement { tag: "Community size";  xml:"contactpool_info.community.size"; fract: false}


                        }

                        Repeater{
                            model: num_bttns
                            RowLayout{
                                z:5
                                spacing: 50
                                Layout.alignment: Qt.AlignTop
                                Layout.maximumHeight: 4
                                Label{
                                    id: label
                                    text: tag
                                    //lineHeight: 0.9
                                    verticalAlignment: Text.AlignVCenter
                                    Layout.minimumWidth: 200
                                    Layout.alignment: Qt.AlignLeft
                                }

                                SpinBox {
                                    id: spinBox9
                                    font.pointSize: 10
                                    Layout.minimumWidth: 100
                                    stepSize: 1
                                    maximumValue: fract ? 100 : Infinity
                                    //Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignRight
                                    value: fract? backend.get_config(xml) * 100 : backend.get_config(xml)
                                    suffix: fract ? "%" : ""
                                    onValueChanged: fract? backend.set_config(xml, value / 100): backend.set_config(xml, value)
                                }

                            }
                        }
                        RowLayout{
                            Layout.alignment: Qt.AlignTop
                            Layout.maximumHeight: 4
                            property string tag : "cities"

                            Label{
                                id: cty_file_label
                                text: "City data file"
                                verticalAlignment: Text.AlignVCenter
                                Layout.minimumWidth: 245
                                Layout.alignment: Qt.AlignLeft

                            }

                            TextField{
                                 id: cty_config_text
                                 Layout.alignment: Qt.AlignRight
                                 Layout.minimumWidth: 300
                                 text: backend.read_path(parent.tag)


                            }

                            Button{
                                text: "..."
                                onClicked:{
                                    console.log(backend.read_path("cities"))
                                    cty_file.visible = true
                                }
                                Layout.maximumWidth: 20
                            }

                        }



                        FileDialog{
                            id: cty_file
                            title: "Please choose a file"
                            folder: "../data/"
                             nameFilters: [ "*.csv"]
                                onAccepted: {
                                   console.log("You chose: " + cty_file.fileUrls)
                                   cty_config_text.text = cty_file.fileUrls.toString()


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
