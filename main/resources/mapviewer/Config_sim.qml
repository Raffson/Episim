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

            ListModel{
                id: sim_spnners
                ListElement { txt: "Number of days"; tagi: "run.num_days"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Number of threads"; tagi: "run.num_threads"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Number participants survey"; tagi: "run.num_participants_survey"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "RNG seed"; tagi: "run.rng_seed"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "r0"; tagi: "run.r0"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Seeding age min"; tagi: "run.seeding_age_min"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Seeding age max"; tagi: "run.seeding_age_max"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}

            }
            
            Repeater{
               model:sim_spnners
               Spnner_label{

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

            ListModel{
                id: sim_cnfg_file
                ListElement{ tg: "run.age_contact_matrix_file"; txt: "Age contact matrix file"; xml: true ; grid: false; dta: true}
                ListElement{ tg: "run.disease_config_file"; txt: "Disease config file"; xml: true ; grid: false; dta: true}
                ListElement{ tg: "run.geopop_file"; txt: "Geopop config file"; xml: true ; grid: false; dta: false}
                ListElement{ tg: "run.population_file"; txt: "Population file"; xml: true ; grid: false; dta: false}

            }
            
            Repeater{
                model: sim_cnfg_file

                File_select{
                    id: cty_fil_select
                }
                
            }
        }
    }
}
