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
        ScrollView{
            anchors.fill: parent
            anchors.margins: 25
        Grid{
            id: alligner
            layoutDirection: Qt.LeftToRight
            flow: GridLayout.TopToBottom
            anchors.topMargin: parent.height / 50
            anchors.leftMargin: 50
            columnSpacing: 50
            rowSpacing: 25
            rows: 8
            columns: 2
            ListModel{
                id: num_bttns

                ListElement { txt: "Population"; tagi: "pop_info.pop_total"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: true}
                ListElement { txt: "Fraction students"; tagi: "pop_info.fraction_students"; decims: 2; stpsize: 0.1; mn: 0; mx : 1; geo: true}
                ListElement { txt: "Fraction active workers"; tagi: "pop_info.fraction_active_workers"; decims: 2; stpsize: 0.1; mn: 0; mx : 1; geo: true}
                ListElement { txt: "Fraction commuting students"; tagi: "pop_info.fraction_commuting_workers"; decims: 2; stpsize: 0.1; mn: 0; mx : 1; geo: true}
                ListElement { txt: "Fraction commuting workers"; tagi: "pop_info.fraction_commuting_workers"; decims: 2; stpsize: 0.1; mn: 0; mx : 1; geo: true }
                ListElement { txt: "Average size"; tagi:"contactpool_info.average_size"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: true}
                ListElement { txt: "School Size";  tagi:"contactpool_info.school.size";decims: 0; stpsize: 1; mn: 0; mx : -1; geo: true }
                ListElement { txt: "College Size"; tagi:"contactpool_info.college.size";decims: 0; stpsize: 1; mn: 0; mx : -1; geo: true }
                ListElement { txt: "Workplace Size"; tagi:"contactpool_info.workplace.size";decims: 0; stpsize: 1; mn: 0; mx : -1; geo: true}
                ListElement { txt: "Nr cities with colleges"; tagi:"contactpool_info.college.cities"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: true}
                ListElement { txt: "Community size";  tagi:"contactpool_info.community.size"; decims: 0; stpsize: 1; mn: 0; mx : -1; geo: true}

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

            ListModel{
                id: check_rndm_ages
                ListElement{ txt: "Random ages"; tagi: "pop_info.random_ages"; geo: true}
            }
            
            Repeater{
                model: check_rndm_ages

                Checker {
                    id: checkRandomAges
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
                    text: "Defrag cities"
                }

                Label{
                    verticalAlignment: Text.AlignVCenter
                    Layout.alignment: Qt.AlignLeft
                    text: "Enabled"
                }

                CheckBox{
                    id: c
                    checked: backend.getBoolConfig("defrag_cities.is_defrag")
                    onClicked:{
                        backend.setBoolConfig("defrag_cities.is_defrag", checked, true);
                        rep.model = p_vec_size.value;
                    }
                }

                Label{
                    verticalAlignment: Text.AlignVCenter
                    Layout.alignment: Qt.AlignLeft
                    text: "X"
                }


                SpinBox {
                    id: spinBox9
                    font.pointSize: 10
                    Layout.minimumWidth: 25
                    Layout.alignment: Qt.AlignRight
                    enabled: c.checked ? true: false
                    stepSize: 0.1
                    decimals: 2
                    minimumValue: 0
                    maximumValue: 1
                    value:backend.getConfig("defrag_cities.X")
                    onValueChanged: {backend.setConfig("defrag_cities.X", value) }
                }

                Label{
                    verticalAlignment: Text.AlignVCenter
                    Layout.alignment: Qt.AlignLeft
                    text: "Y"
                }

                SpinBox {
                    font.pointSize: 10
                    Layout.minimumWidth: 25
                    Layout.alignment: Qt.AlignRight
                    enabled: c.checked ? true: false
                    stepSize: 0.1
                    decimals: 2
                    minimumValue: 0
                    maximumValue: 1
                    value:backend.getConfig("defrag_cities.Y")
                    onValueChanged: backend.setConfig("defrag_cities.Y", value)
                }

                Label{
                    verticalAlignment: Text.AlignVCenter
                    Layout.alignment: Qt.AlignLeft
                    text: "p_vec size"
                }

                SpinBox {
                    id: p_vec_size
                    font.pointSize: 10
                    Layout.minimumWidth: 50
                    Layout.alignment: Qt.AlignTop
                    enabled: c.checked ? true: false
                    stepSize: 1
                    decimals: 0
                    minimumValue: 1
                    maximumValue: Infinity
                    value:{(backend.countConfigChildren("defrag_cities.p_vec", true) ) }
                    onValueChanged:{rep.model = value;
                        backend.cleanChildren("defrag_cities.p_vec", value, true)
                    }
                }


                GridLayout{
                    columns: 3
                    rows: 5
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                Repeater{
                    id: rep
                    model:{ (p_vec_size.value - 1 )}

                        RowLayout{
                        Label{
                            verticalAlignment: Text.AlignVCenter
                            Layout.alignment: Qt.AlignLeft
                            text: "p_val " + (index + 2) + " cities"
                            enabled: c.checked ? true: false
                        }

                        SpinBox {
                            font.pointSize: 10
                            Layout.minimumWidth: 50
                            Layout.alignment: Qt.AlignRight
                            enabled: c.checked ? true: false
                            stepSize: 0.1
                            decimals: 2
                            minimumValue: 0
                            maximumValue: 1
                            value:{ backend.getChildAtIndex("defrag_cities.p_vec", index, true)}
                            onValueChanged: backend.setChildAtIndex("defrag_cities.p_vec", index, value,true)
                        }
                        }
                    }
                }
            }
        }
    }
}
}



