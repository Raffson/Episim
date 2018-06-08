import QtQuick 2.1
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.11
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.4
//import episim.city 1.0


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

                Item {
                    id: item1
                    anchors.leftMargin: parent.width / 25
                    anchors.topMargin: parent.height / 25
                    anchors.fill: parent

                    ColumnLayout{
                        id: alligner
                        width: parent.width
                        RowLayout{
                            id: row1
                            spacing: 25
                            Layout.fillHeight: true


                            Text {
                                id: text1
                                text: qsTr("Population:")
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 20

                            }

                            ColumnLayout {
                                id: pop_grp
                                spacing: 5
                                Layout.fillWidth: true
                                Label {
                                    id: label
                                    text: qsTr("Population")
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignVCenter
                                }


                                TextField {
                                    id: textField
                                    width: parent.width / 6
                                    placeholderText: qsTr("Text Field")
                                    //Layout.fillWidth: true
                                }

                            }

                            ColumnLayout {
                                id: fract
                                Layout.fillWidth: true
                                Label {
                                    id: label1
                                    text: qsTr("Students (%)")
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox
                                    editable: true
                                    stepSize: 0
                                    to: 1
                                }

                                spacing: 5
                            }

                            ColumnLayout {
                                id: fract1
                                Label {
                                    id: label2
                                    text: qsTr("Commuting Students(%)")
                                    font.pointSize: 10
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox1
                                    stepSize: 0
                                    editable: true
                                    to: 1
                                }
                                spacing: 5
                            }

                            ColumnLayout {
                                id: fract2
                                Label {
                                    id: label3
                                    text: qsTr("Active Workers(%)")
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox2
                                    stepSize: 0
                                    editable: true
                                    to: 1
                                }
                                spacing: 5
                            }

                            ColumnLayout {

                                id: fract3
                                spacing: 5
                                Label {
                                    id: label4
                                    text: qsTr("Active Workers(%)")
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox3
                                    width: parent.width
                                    stepSize: 0
                                    editable: true
                                    to: 1
                                }

                            }

                        }
                        RowLayout{
                            id: contactpool_info
                            spacing: 25
                            Text {
                                id: cp_text1
                                text: qsTr("Population:")
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 20

                            }

                            ColumnLayout {
                                id: cp_pop_grp
                                spacing: 5
                                Layout.fillWidth: true
                                Label {
                                    id: cp_label
                                    text: qsTr("Population")
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignVCenter
                                }


                                TextField {
                                    id: cp_textField
                                    width: parent.width / 6
                                    placeholderText: qsTr("Text Field")
                                    //Layout.fillWidth: true
                                }

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
