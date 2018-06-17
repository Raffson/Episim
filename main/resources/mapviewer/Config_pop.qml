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

                ListElement { txt: "Population"; tagi: "pop_info.pop_total"; decims: 0; stpsize: 1; mn: 0; mx : -1}
                ListElement { txt: "Fraction students"; tagi: "pop_info.fraction_students"; decims: 2; stpsize: 0.1; mn: 0; mx : 1}
                ListElement { txt: "Fraction active workers"; tagi: "pop_info.fraction_active_workers"; decims: 2; stpsize: 0.1; mn: 0; mx : 1}
                /*

                ListElement { tag: "Fraction commuting students"; xml: "pop_info.fraction_commuting_workers"; fract: true}
                ListElement { tag: "Fraction commuting workers"; xml: "pop_info.fraction_commuting_workers"; fract: true}
                ListElement { tag: "Average size"; xml:"contactpool_info.average_size"; fract: false }
                ListElement { tag: "School Size";  xml:"contactpool_info.school.size";fract: false }
                ListElement { tag: "College Size"; xml:"contactpool_info.college.size";fract: false }
                ListElement { tag: "Workplace Size";  xml:"contactpool_info.workplace.size";fract: false }
                ListElement { tag: "Nr cities with colleges"; xml:"contactpool_info.college.cities"; fract: false}
                ListElement { tag: "Community size";  xml:"contactpool_info.community.size"; fract: false}*/
                     
            }
            
            Repeater{
                model: num_bttns
                Spnner_label {
                }
            }

            ListModel{
                id: search_file_model
                ListElement{ tg: "data_files.cities"; txt: "City data file"; xml: false ; grid: true; dta: true}
                ListElement{ tg: "data_files.commuting"; txt: "Commuters data file"; xml: false; grid:true; dta: true}
                ListElement{ tg: "data_files.households"; txt: "Households data file"; xml: true; grid:true; dta: true}
                
            }
            
            Repeater{
                model: search_file_model
                File_select {
                    id: cty_fil_select
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
