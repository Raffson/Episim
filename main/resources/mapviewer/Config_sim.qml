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
            rows: 15
            columns: 2

            ListModel{
                id: sim_spnners
                ListElement { txt: "Number of days"; tagi: "run.num_days"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Number of threads"; tagi: "run.num_threads"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Number participants survey"; tagi: "run.num_participants_survey"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "RNG seed"; tagi: "run.rng_seed"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "r0"; tagi: "run.r0"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Seeding age min"; tagi: "run.seeding_age_min"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: false}
                ListElement { txt: "Immunity rate"; tagi: "run.immunity_rate"; decims: 2; stpsize: 0.1; mn: 0; mx : 1; geo: false}
                ListElement { txt: "Seeding rate"; tagi: "run.seeding_rate"; decims: 3; stpsize: 0.001; mn: 0; mx : 1; geo: false}
                ListElement { txt: "Vaccine link probability"; tagi: "run.vaccine_link_probability"; decims: 2; stpsize: 0.01; mn: 0; mx : 1; geo: false}
                ListElement { txt: "Vaccine rate"; tagi: "run.vaccine_rate"; decims: 2; stpsize: 0.1; mn: 0; mx : 1; geo: false}
            }
            
            Repeater{
               model:sim_spnners
               Spnner_label{

               }
            }

            Repeater{
                
                model:[ {txt: "Behaviour policy", tg: "run.behaviour_policy", mdl:["Vaccination" ,"NoBehaviour"]},
                    {txt: "Belief policy",tg: "run.belief_policy", mdl:["Belief", "HBM", "Imitation", "NoBelief", "Treshold"]},
                    {txt: "Global information policy",tg: "run.global_information_policy", mdl:["LocalDiscussion", "NoGlobalInformation", "NoLocalInformation"]},
                    {txt: "Contact log level", tg: "run.contact_log_level", mdl: ["None", "Tansimissions", "All", "Susceptibles"]},
                    {txt: "Immunity profile",tg: "run.immunity_profile", mdl:  ["None"]},
                    {txt: "Vaccine profile", tg: "run.vaccine_profile", mdl: ["Random"]},
                    {txt: "Rng type", tg: "run.rng_type", mdl: ["mrg2", "mrg3", "lgc64", "lcg64_shift", "yarn2", "yarn3"]},
                    {txt: "Stride log level", tg: "run.stride_log_level", mdl:["Info"]}
                    
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
                        editable: true
                        id: belief_txt
                        Layout.minimumWidth: 200
                        model: modelData.mdl
                        currentIndex: find(backend.getConfig(modelData.tg, false))
                        //Layout.fillWidth: true
                        Layout.alignment: Qt.AlignRight
                        Layout.maximumHeight: 25

                        onCountChanged:currentIndex = find(backend.getConfig(modelData.tg, false))
                        onCurrentTextChanged: backend.setConfig(modelData.tg, currentText, false)
                        
                    }

                    
                }
            }

            ListModel{
                id: stride_checkers
                ListElement{ txt: "Contact output file"; tagi: "run.contact_output_file"; geo: false}
                ListElement{ txt: "Output adopted"; tagi: "run.output_adopted"; geo: false}
                ListElement{ txt: "Output cases"; tagi: "run.output_cases"; geo: false}
                ListElement{ txt: "Output persons"; tagi: "run.output_persons"; geo: false}
                ListElement{ txt: "Output summary"; tagi: "run.output_summary"; geo: false}
                ListElement{ txt: "Track index case"; tagi: "run.track_index_case"; geo: false}
                ListElement{ txt: "Use install dirs"; tagi: "run.use_install_dirs"; geo: false}
            }
            
            Repeater{
                
                model: stride_checkers

                Checker{

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
