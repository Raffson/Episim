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

    Component{
        id: lbl
        Label{
            id: label
            lineHeight: 0.9
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }


    }

    Component{
        id:spnner
        SpinBox {
            id: spinBox9
            to: 1
            stepSize: 0
            editable: true
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
                    rows: 2
                    columns: 5
                    Repeater{
                        model: ["Population", "Fractions Students", "Fraction commuting students", "Fraction active workers"
                                ,"Fraction commuting workers", "Average size", "College Size", "Community size", "Workplace Size"]
                            ColumnLayout{
                                Layout.alignment: Qt.AlignTop
                                Label{
                                    id: label
                                    text: modelData
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignVCenter
                                    Layout.fillWidth: true
                                }

                                SpinBox {
                                    id: spinBox9
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }

                            }
                    }



                            /*ColumnLayout {
                                id: pop_grp
                                spacing: 5
                                Layout.fillWidth: true
                                Label {
                                    id: label
                                    text: qsTr("Population")
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignVCenter
                                    Layout.fillWidth: true
                                }

                                SpinBox {
                                    id: spinBox9
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }


                            }

                            ColumnLayout {
                                id: fract
                                Layout.fillWidth: true
                                Label {
                                    id: label1
                                    text: qsTr("Students (%)")
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignLeft
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox
                                    editable: true
                                    stepSize: 0
                                    to: 1
                                    Layout.fillWidth: true
                                }

                                spacing: 5
                            }

                            ColumnLayout {
                                id: fract1
                                Label {
                                    id: label2
                                    text: qsTr("Commuting Students(%)")
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignLeft
                                    font.pointSize: 12
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox1
                                    stepSize: 0
                                    editable: true
                                    to: 1
                                    Layout.fillWidth: true
                                }
                                spacing: 5
                            }

                            ColumnLayout {
                                id: fract2
                                Label {
                                    id: label3
                                    text: qsTr("Active Workers(%)")
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignLeft
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox2
                                    stepSize: 0
                                    editable: true
                                    to: 1
                                    Layout.fillWidth: true
                                }
                                spacing: 5
                            }

                            ColumnLayout {

                                id: fract3
                                Layout.fillWidth: false
                                spacing: 5
                                Label {
                                    id: label4
                                    text: qsTr("Active Workers(%)")
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignLeft
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop

                                }

                                SpinBox {
                                    id: spinBox3
                                    width: parent.width
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }

                            }

                            ColumnLayout {
                                id: average_size
                                Label {
                                    id: com_size
                                    text: qsTr("Average Size")
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignLeft
                                    Layout.minimumWidth: 50
                                    Layout.preferredWidth: 50
                                    lineHeight: 0.9
                                    verticalAlignment: Text.AlignTop
                                }

                                SpinBox {
                                    id: spinBox4
                                    width: parent.width
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }
                                spacing: 5
                            }

                            ColumnLayout {
                                id: school_size
                                Label {
                                    id: com_size1
                                    text: qsTr("School Size")
                                    Layout.fillWidth: true
                                    lineHeight: 0.9
                                    horizontalAlignment: Text.AlignLeft
                                    Layout.preferredWidth: 50
                                    verticalAlignment: Text.AlignTop
                                    Layout.minimumWidth: 50
                                }

                                SpinBox {
                                    id: spinBox5
                                    width: parent.width
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }
                                spacing: 5
                            }

                            ColumnLayout {
                                id: college_size
                                Label {
                                    id: com_size2
                                    text: qsTr("College Size")
                                    Layout.fillWidth: true
                                    lineHeight: 0.9
                                    horizontalAlignment: Text.AlignLeft
                                    Layout.preferredWidth: 50
                                    verticalAlignment: Text.AlignTop
                                    Layout.minimumWidth: 50
                                }

                                SpinBox {
                                    id: spinBox6
                                    width: parent.width
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }
                                spacing: 5
                            }

                            ColumnLayout {
                                id: workplace_size
                                Label {
                                    id: com_size3
                                    text: qsTr("Workplace Size")
                                    Layout.fillWidth: true
                                    lineHeight: 0.9
                                    horizontalAlignment: Text.AlignLeft
                                    Layout.preferredWidth: 50
                                    verticalAlignment: Text.AlignTop
                                    Layout.minimumWidth: 50
                                }

                                SpinBox {
                                    id: spinBox7
                                    width: parent.width
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }
                                spacing: 5
                            }

                            ColumnLayout {
                                id: community_size
                                Label {
                                    id: com_size4
                                    text: qsTr("Community Size")
                                    Layout.fillWidth: true
                                    lineHeight: 0.9
                                    horizontalAlignment: Text.AlignLeft
                                    Layout.preferredWidth: 50
                                    verticalAlignment: Text.AlignTop
                                    Layout.minimumWidth: 50
                                }

                                SpinBox {
                                    id: spinBox8
                                    width: parent.width
                                    to: 1
                                    stepSize: 0
                                    editable: true
                                    Layout.fillWidth: true
                                }
                                spacing: 5
                            }*/

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
