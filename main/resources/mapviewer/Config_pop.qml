import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

Tab{
    id: config_pop
    Rectangle{
        anchors.fill: parent
        color: Qt.rgba(0.9,0.9,0.9,0.5)
        GridLayout{
            id: alligner
            layoutDirection: Qt.LeftToRight
            flow: GridLayout.TopToBottom
            anchors.topMargin: parent.height / 50
            anchors.leftMargin: 50
            anchors.fill: parent
            columnSpacing: 5
            rowSpacing: 1
            rows: 15
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
                        value: fract? backend.getConfig(xml) * 100 : backend.getConfig(xml)
                        suffix: fract ? "%" : ""
                        onValueChanged: fract? backend.setConfig(xml, value / 100): backend.setConfig(xml, value)
                    }
                    
                }
            }
            
            
            
            ListModel{
                id: search_file_model
                ListElement{ tg: "cities"; txt: "City data file"; def_folder: "../data/"; xml: false}
                ListElement{ tg: "commuting"; txt: "Commuters data file"; def_folder: "../data/";xml: false}
                ListElement{ tg: "households"; txt: "Households data file"; def_folder: "../data/"; xml: true}
                
            }
            
            Repeater{
                model: search_file_model
                RowLayout{
                    id: cty_fil_select
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: 4
                    property string tag : tg
                    
                    Label{
                        id: cty_file_label
                        text: txt
                        verticalAlignment: Text.AlignVCenter
                        Layout.minimumWidth: 245
                        Layout.alignment: Qt.AlignLeft
                        
                    }
                    
                    TextField{
                        id: cty_config_text
                        Layout.alignment: Qt.AlignRight
                        Layout.minimumWidth: 300
                        text: backend.readPath(parent.tag)
                        
                        
                    }
                    
                    Button{
                        text: "..."
                        onClicked:{
                            cty_file.visible = true
                        }
                        Layout.maximumWidth: 20
                    }
                    
                    FileDialog{
                        id: cty_file
                        title: "Please choose a file"
                        folder: def_folder
                        nameFilters: xml ?["*.xml"] : [ "*.csv"]
                        onAccepted: {
                            cty_config_text.text = backend.setPath(cty_fil_select.tag, cty_file.fileUrls.toString());
                            
                        }
                        onRejected: {
                            
                        }
                        
                    }
                }
                
            }
            
            RowLayout{
                Layout.alignment: Qt.AlignTop
                Layout.maximumHeight: 4
                Label{
                    id: rndm_age_lbl
                    verticalAlignment: Text.AlignVCenter
                    Layout.minimumWidth: 245
                    Layout.alignment: Qt.AlignLeft
                    text: "Random ages"
                }
                
                CheckBox{
                    id: rndm_age
                    checked: backend.getBoolConfig("pop_info.random_ages")
                    onClicked:backend.setBoolConfig("pop_info.random_ages", checked);
                }
            }
            
        }
    }
}
