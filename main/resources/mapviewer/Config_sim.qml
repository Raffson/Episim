import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

Tab{
    id: config_sim
    title:"Config sim"
    Rectangle{
        anchors.fill: parent
        color: Qt.rgba(0.9,0.9,0.9,0.5)
        GridLayout{
            id: alligner_run
            layoutDirection: Qt.LeftToRight
            flow: GridLayout.TopToBottom
            anchors.topMargin: parent.height / 50
            anchors.leftMargin: 50
            anchors.fill: parent
            columnSpacing: 50
            rowSpacing: 5
            rows: 19
            columns: 2
            
            Repeater{
                
                model:[ {txt: "Number of days"},
                    {txt: "Number participants survey"},
                    {txt: "number of threads"},
                    {txt: "RNG seed"},
                    {txt: "r0"},
                    {txt: "Seeding age min"},
                    {txt: "Seeding age max"},
                ]
                
                delegate :RowLayout{
                    z:5
                    spacing: 50
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: 4
                    Label{
                        id: integer_spinnr_txt
                        text: modelData.txt
                        //lineHeight: 0.9
                        verticalAlignment: Text.AlignVCenter
                        Layout.minimumWidth: 200
                        Layout.alignment: Qt.AlignLeft
                    }
                    
                    SpinBox{
                        id: integer_spinner
                        Layout.minimumWidth: 100
                        //Layout.fillWidth: true
                        Layout.alignment: Qt.AlignRight
                        Layout.maximumHeight: 25
                        
                    }
                    
                }
            }
            
            Repeater{
                
                model:[{txt: "immunity rate"},
                    {txt: "Seeding rate"},
                    {txt: "Vaccine link probability"},
                    {txt: "Vaccine rate"}
                ]
                
                delegate :RowLayout{
                    z:5
                    spacing: 50
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: 4
                    Label{
                        id: fract_spinnr_txt
                        text: modelData.txt
                        //lineHeight: 0.9
                        verticalAlignment: Text.AlignVCenter
                        Layout.minimumWidth: 200
                        Layout.alignment: Qt.AlignLeft
                    }
                    
                    SpinBox{
                        id: float_spinner
                        Layout.minimumWidth: 100
                        //Layout.fillWidth: true
                        Layout.alignment: Qt.AlignRight
                        Layout.maximumHeight: 25
                        
                        minimumValue: 0.0
                        maximumValue: 1.0
                        stepSize: 0.1
                        decimals: 3
                        
                    }
                    
                }
            }
            
            
            Repeater{
                
                model:[ {txt: "Behaviour policy", mdl:["NoBehaviour", "Vaccination" ]},
                    {txt: "Belief policy", mdl:["Belief", "HBM", "Imitation", "NoBelief", "Treshold"]},
                    {txt: "Information policy", mdl:["LocalDiscussion", "NoGlobalInformation", "NoLocalInformation"]},
                    {txt: "Contact log level", mdl: ["None", "Tansimissions", "All", "Susceptibles"]},
                    {txt: "Immunity profile", mdl:  ["None"]},
                    {txt: "Vaccine profile", mdl: ["Random"]},
                    {txt: "Rng type", mdl: ["mrg2", "mrg3", "lgc64", "lcg64_shift", "yarn2", "yarn3"]},
                    {txt: "Stride log level", mdl:["Info"]}
                    
                ]
                
                delegate :RowLayout{
                    z:5
                    spacing: 50
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: 4
                    Label{
                        id: belief_polic
                        text: modelData.txt
                        //lineHeight: 0.9
                        verticalAlignment: Text.AlignVCenter
                        Layout.minimumWidth: 200
                        Layout.alignment: Qt.AlignLeft
                    }
                    
                    ComboBox{
                        id: belief_txt
                        Layout.minimumWidth: 200
                        model: modelData.mdl
                        //Layout.fillWidth: true
                        Layout.alignment: Qt.AlignRight
                        Layout.maximumHeight: 25
                        
                    }
                    
                }
            }
            
            Repeater{
                
                model:[ {txt: "Contact output file"},
                    {txt: "Output adopted"},
                    {txt: "Output cases"},
                    {txt: "Output persons"},
                    {txt: "Output summary"},
                    {txt: "Track index case"},
                    {txt: "Use install dirs"}
                ]
                
                delegate :RowLayout{
                    z:5
                    spacing: 50
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: 4
                    Label{
                        id: checkboxes
                        text: modelData.txt
                        //lineHeight: 0.9
                        verticalAlignment: Text.AlignVCenter
                        Layout.minimumWidth: 200
                        Layout.alignment: Qt.AlignLeft
                    }
                    
                    CheckBox{
                        id: checkbox_box
                        Layout.minimumWidth: 200
                        //Layout.fillWidth: true
                        Layout.alignment: Qt.AlignRight
                        Layout.maximumHeight: 25
                        
                    }
                    
                }
                
                
            }
            
            
            RowLayout{
                
                z:5
                spacing: 5
                Layout.alignment: Qt.AlignTop
                Layout.maximumHeight: 4
                Label{
                    id: ll
                    text: "Date"
                    //lineHeight: 0.9
                    verticalAlignment: Text.AlignVCenter
                    Layout.minimumWidth: 246
                    Layout.alignment: Qt.AlignLeft
                }
                
                Label{
                    id: day
                    text: "Day"
                    //lineHeight: 0.9
                    verticalAlignment: Text.AlignVCenter
                    Layout.minimumWidth: 30
                    Layout.maximumWidth: 30
                    Layout.alignment: Qt.AlignLeft
                }
                SpinBox{
                    id: day_spnner
                    Layout.minimumWidth: 25
                    //Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    Layout.maximumHeight: 25
                    
                    minimumValue: 0.0
                    maximumValue: 31
                    stepSize: 1
                    decimals: 0
                    
                }
                
                Label{
                    id: month
                    text: "Month"
                    //lineHeight: 0.9
                    verticalAlignment: Text.AlignVCenter
                    Layout.minimumWidth: 45
                    Layout.maximumWidth: 45
                    Layout.alignment: Qt.AlignLeft
                }
                SpinBox{
                    id: month_spnner
                    Layout.minimumWidth: 25
                    //Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    Layout.maximumHeight: 25
                    
                    minimumValue: 0
                    maximumValue: 12
                    stepSize: 1
                    decimals: 0
                    
                }
                
                Label{
                    id: year
                    text: "Year"
                    //lineHeight: 0.9
                    verticalAlignment: Text.AlignVCenter
                    Layout.minimumWidth: 30
                    Layout.maximumWidth: 30
                    Layout.alignment: Qt.AlignLeft
                }
                SpinBox{
                    id: year_spnner
                    Layout.minimumWidth: 25
                    //Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    Layout.maximumHeight: 25
                    
                    minimumValue: -Infinity
                    maximumValue: Infinity
                    stepSize: 1
                    decimals: 0
                    
                }
                
            }
            
            Repeater{
                model:
                    [
                    {txt: "Age contact matrix file"},
                    {txt: "Disease config file"},
                    {txt: "Geopop config file"},
                    {txt: "Population file"}
                ]
                RowLayout{
                    id: cty_fil_selector
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: 4
                    //property string tag : tg
                    
                    Label{
                        id: run_config_files
                        text: modelData.txt
                        verticalAlignment: Text.AlignVCenter
                        Layout.minimumWidth: 245
                        Layout.alignment: Qt.AlignLeft
                        
                    }
                    
                    TextField{
                        id: run_config_txt
                        Layout.alignment: Qt.AlignRight
                        Layout.minimumWidth: 300
                        text: backend.readPath(parent.tag)
                        
                        
                    }
                    
                    Button{
                        text: "..."
                        onClicked:{
                            //cty_file.visible = true
                        }
                        Layout.maximumWidth: 20
                    }
                    
                    FileDialog{
                        id: run_file
                        title: "Please choose a file"
                        //folder: def_folder
                        //nameFilters: xml ?["*.xml"] : [ "*.csv"]
                        onAccepted: {
                            //cty_config_text.text = backend.setPath(cty_fil_select.tag, cty_file.fileUrls.toString());
                            
                        }
                        onRejected: {
                            
                        }
                        
                    }
                }
                
            }
        }
    }
}
