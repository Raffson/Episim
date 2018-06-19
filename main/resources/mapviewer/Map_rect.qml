import QtQuick 2.0
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.4




Rectangle{
    Plugin {
        id: mapPlugin
        name: "mapbox"//", "esri", ...
        PluginParameter{ name: "mapbox.access_token";
            value: "pk.eyJ1Ijoicm9iYmVoZWlybWFuIiwiYSI6ImNqaTBhYWY2bjEyZG8zcHBncmN5amc4ajUifQ.IiFHtjyHqO0Mrl5Xz_5aug"
        }


    }
    id: map_rect
    anchors.fill: parent

    ToolBar {
        id: toolBar
        opacity: 1
        anchors.top: parent.top
        width: parent.width
        height: 25
        RowLayout{
            spacing: 10
            anchors.fill: parent

            Text {
                id: selected_pop_header
                text: "Selected pop: " + backend.selected_pop
                verticalAlignment: Text.AlignVCenter
                //Layout.fillWidth: true
                Layout.preferredWidth: parent.width / 5
            }

            Text {
                id: total_infected
                property int perc: backend.selected_pop === 0 ? 0 : Math.round(backend.selected_infected / backend.selected_pop * 100)
                text: "Selected infected: " + backend.selected_infected + "|"+ perc + "%"
                verticalAlignment: Text.AlignVCenter
                //Layout.fillWidth: true

            }

            Text{
                verticalAlignment: Text.AlignVCenter
                text: "Total pop: " + backend.total_pop

            }

            Text{
                verticalAlignment: Text.AlignVCenter
                property real perc: backend.total_pop === 0 ? 0 : Math.round(backend.total_infected / backend.total_pop * 100)
                text: "Total infected: " + backend.total_infected + "|" + perc + "%"
            }


            Button {
                Layout.fillHeight: true
                id: button
                text: qsTr("RUN PopBuilder")

                onClicked: {
                    map.clearMapItems();
                    backend.genPop();
                    gc();
                    map.center_and_zoom();

                }

                background: Rectangle {
                    id: pop_b
                    opacity: enabled ? 1 : 0.3
                    color: button.down ?  Qt.rgba(0,0,1,0.2) : button.hovered ? Qt.rgba(0,0,0,0.2) : Qt.rgba(0,1,0,0.2)
                }
            }
            Button {
                id: button_sim
                text: qsTr("RUN sim")
                onClicked:{
                    var zm = false;
                    if(backend.shouldRedraw()){
                        map.clearMapItems();
                        zm = true
                    }

                    backend.runSimulator(run_sim_days.value, rn_checker.checked);

                    if(zm){
                        map.center_and_zoom();
                    }


                }

                background: Rectangle {
                    id: run_sim
                    opacity: enabled ? 1 : 0.3
                    color: button_sim.down ?  Qt.rgba(0,0,1,0.2) : button_sim.hovered ? Qt.rgba(0,0,0,0.2) : Qt.rgba(0,1,0,0.2)
                }

            }
            RowLayout{
                Label{
                    text: "All:"
                }
                CheckBox{
                    id: rn_checker
                    Layout.fillHeight: true
                }
            }
            RowLayout{
                Label{
                    text: "Days:"
                }

                SpinBox{
                    id: run_sim_days
                    Layout.fillHeight: true
                    editable: true
                    enabled: !rn_checker.checked
                }
            }
        }
    }

    Rectangle{
        id: sidebar
        anchors.left: parent.left
        anchors.top: toolBar.bottom
        anchors .right: map.left
        anchors.bottom: map.bottom
        width: parent.width / 5
        Rectangle{
            id: title
            anchors.bottom: scrollie.top
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            width: parent.width
            height: 30
            color: Qt.rgba(0.2,0.2,0.2,0.2)
        }
        Text{
            anchors.fill: parent
            text: "Selected Cities"
            font.pointSize: 15
            horizontalAlignment: Text.AlignHCenter
        }
        ScrollView{
            id: scrollie
            clip: true
            anchors.top: title.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            Column{
                spacing: 10
                Repeater{
                    model: backend.cities
                    Rectangle{
                        id: base_rec
                        color: Qt.rgba(0.7,0.7,0.7)
                        visible: modelData.clicked ? true: false
                        width: sidebar.width
                        height: modelData.clicked ? main_col.collapsed_height : 0
                        Column{
                            id: main_col
                            anchors.fill : parent
                            property int uncollapsed_height: 300
                            property int collapsed_height:30
                            property bool collapsed: true
                            spacing: 10
                            Item{
                                height: parent.collapsed_height
                                width : parent.width
                                Row{
                                    spacing: 5
                                    anchors.fill: parent
                                    Button{
                                        width: 15
                                        height: 15
                                        id:drpdown_city
                                        flat: true
                                        onClicked:{
                                            main_col.collapsed = !main_col.collapsed
                                            if(main_col.collapsed){
                                              base_rec.height -= sub_row.height
                                              sub_row.height = 0
                                              sub_row.visible = false

                                            }
                                            else{
                                                sub_row.height = main_col.uncollapsed_height - main_col.collapsed_height
                                                base_rec.height += sub_row.height
                                                sub_row.visible = true
                                            }
                                        }
                                        contentItem: Text {
                                            anchors.fill: parent
                                            text: main_col.collapsed ? "^" : ">"
                                            verticalAlignment: Text.AlignVCenter
                                            font.pointSize: 15
                                            //horizontalAlignment: Text.AlignVCenter
                                            //verticalAlignment:  Text.AlignVCenter
                                            color: drpdown_city.down ? "white": "green"
                                        }
                                    }
                                    Text{
                                        text: modelData.name

                                    }

                                    Button{
                                        width: 15
                                        height: 15
                                        id:b
                                        flat: true
                                        onClicked: modelData.clicked = false
                                        contentItem: Text {
                                            anchors.fill: parent
                                            text: qsTr("x")
                                            font.pointSize: 15
                                            color: b.down ? "white": "red"
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                    }
                                }
                            }

                            Item{
                                id: sub_row
                                height: main_col.uncollapsed_height - main_col.collapsed_height
                                width: parent.width
                                visible: false
                                Column{
                                    spacing: 20
                                    anchors.fill: parent
                                    //anchors.fill: parent
                                    Text{
                                        height: parent.height / parent.children.length
                                        text: "Population: " + modelData.popCount
                                        font.pointSize: 13
                                    }
                                    Text{
                                        height: parent.height / parent.children.length
                                        text: "Infected: " + modelData.infected+ "|" + Math.round(modelData.infected/modelData.popCount*100)+"%"
                                        font.pointSize: 13
                                    }
                                    Row{
                                        id: school_select
                                        height: parent.height / parent.children.length
                                        width: parent.width

                                        Button{
                                            property bool selected: false
                                            width: 15
                                            height: 15
                                            id:drpdown_schools
                                            flat: true
                                            contentItem: Text {
                                                anchors.fill: parent
                                                text:parent.selected ? ">" : "^"
                                                verticalAlignment: Text.AlignVCenter
                                                font.pointSize: 15
                                                //horizontalAlignment: Text.AlignVCenter
                                                //verticalAlignment:  Text.AlignVCenter
                                                color: drpdown_schools.down ? "white": "green"
                                            }

                                            onClicked: {
                                                selected = !selected

                                                if(selected){
                                                    base_rec.height += 36 * modelData.schools.length
                                                }

                                                else{
                                                   base_rec.height -= 36 * modelData.schools.length
                                                }
                                            }
                                        }

                                        Text{
                                            id: parental_school_text
                                            text: "School count: " + modelData.schools.length
                                            font.pointSize: 12
                                        }
                                    }

                                    Repeater{
                                        model: modelData.schools
                                        Text{
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            width: parent.width
                                            anchors.leftMargin: 25
                                            visible: drpdown_schools.selected ? true : false
                                            text: "ID: " + modelData.id + "    " + "Size: " + modelData.pop
                                            font.pointSize: 11//{parental_school_text.pointSize - 2}
                                        }
                                    }

                                    Row{
                                        id: college_select
                                        height: parent.height / parent.children.length
                                        width: parent.width

                                        Button{
                                            property bool selected: false
                                            width: 15
                                            height: 15
                                            id:drpdown_college
                                            flat: true
                                            contentItem: Text {
                                                anchors.fill: parent
                                                text:parent.selected ? ">" : "^"
                                                verticalAlignment: Text.AlignVCenter
                                                font.pointSize: 15
                                                //horizontalAlignment: Text.AlignVCenter
                                                //verticalAlignment:  Text.AlignVCenter
                                                color: drpdown_college.down ? "white": "green"
                                            }

                                            onClicked: {
                                                selected = !selected

                                                if(selected){
                                                    base_rec.height += 36 * modelData.colleges.length
                                                }

                                                else{
                                                   base_rec.height -= 36 * modelData.colleges.length
                                                }
                                            }
                                        }
                                        Text{
                                            text: "College count: " + modelData.colleges.length
                                            font.pointSize: 12
                                        }
                                    }

                                    Repeater{
                                        model: modelData.colleges
                                        Text{
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            width: parent.width
                                            anchors.leftMargin: 25
                                            visible: drpdown_college.selected ? true : false
                                            text: "ID: " + modelData.id + "    " + "Size: " + modelData.pop
                                            font.pointSize: 11//{parental_school_text.pointSize - 2}
                                        }
                                    }

                                    Row{
                                        id: workplace_select
                                        height: parent.height / parent.children.length
                                        width: parent.width

                                        Button{
                                            property bool selected: false
                                            width: 15
                                            height: 15
                                            id:drpdown_workplace
                                            flat: true
                                            contentItem: Text {
                                                anchors.fill: parent
                                                text:parent.selected ? ">" : "^"
                                                verticalAlignment: Text.AlignVCenter
                                                font.pointSize: 15
                                                //horizontalAlignment: Text.AlignVCenter
                                                //verticalAlignment:  Text.AlignVCenter
                                                color: parent.down ? "white": "green"
                                            }

                                            onClicked: {
                                                selected = !selected

                                                if(selected){
                                                    base_rec.height += 36 * modelData.workplaces.length
                                                }

                                                else{
                                                    base_rec.height -= 36 * modelData.workplaces.length
                                                }
                                            }
                                        }
                                        Text{
                                            text: "Workplace count: " + modelData.workplaces.length
                                            font.pointSize: 12
                                        }
                                    }

                                    Repeater{
                                        model: modelData.workplaces
                                        Text{
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            width: parent.width
                                            anchors.leftMargin: 25
                                            visible: drpdown_workplace.selected ? true : false
                                            text:"ID: " + modelData.id + "    " + "Size: " + modelData.pop
                                            font.pointSize: 11//{parental_school_text.pointSize - 2}
                                        }
                                    }
                                    Row{
                                        height: parent.height / parent.children.length
                                        width: parent.width

                                        Button{
                                            property bool selected: false
                                            width: 15
                                            height: 15
                                            id:drpdown_primary
                                            flat: true
                                            contentItem: Text {
                                                anchors.fill: parent
                                                text:parent.selected ? ">" : "^"
                                                verticalAlignment: Text.AlignVCenter
                                                font.pointSize: 15
                                                //horizontalAlignment: Text.AlignVCenter
                                                //verticalAlignment:  Text.AlignVCenter
                                                color: parent.down ? "white": "green"
                                            }

                                            onClicked: {
                                                selected = !selected

                                                if(selected){
                                                    base_rec.height += 36 * modelData.primary_communities.length
                                                }

                                                else{
                                                    base_rec.height -= 36 * modelData.primary_communities.length
                                                }
                                            }

                                        }
                                        Text{
                                            text: "Primary community count: " + modelData.primary_communities.length
                                            font.pointSize: 11
                                        }
                                    }
                                    Repeater{
                                        model: modelData.primary_communities
                                        Text{
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            width: parent.width
                                            anchors.leftMargin: 25
                                            visible: drpdown_primary.selected ? true : false
                                            text: "ID: " + modelData.id + "    " + "Size: " + modelData.pop
                                            font.pointSize: 11//{parental_school_text.pointSize - 2}
                                        }
                                    }

                                    Row{
                                        id: secondary_select
                                        height: parent.height / parent.children.length
                                        width: parent.width

                                        Button{
                                            property bool selected: false
                                            width: 15
                                            height: 15
                                            id:drpdown_secondary
                                            flat: true
                                            contentItem: Text {
                                                anchors.fill: parent
                                                text:parent.selected ? ">" : "^"
                                                verticalAlignment: Text.AlignVCenter
                                                font.pointSize: 15
                                                //horizontalAlignment: Text.AlignVCenter
                                                //verticalAlignment:  Text.AlignVCenter
                                                color: parent.down ? "white": "green"
                                            }
                                            onClicked: {
                                                selected = !selected

                                                if(selected){
                                                    base_rec.height += 36 * modelData.secondary_communities.length
                                                }

                                                else{
                                                    base_rec.height -= 36 * modelData.secondary_communities.length
                                                }
                                            }
                                        }


                                        Text{
                                            text: "Secondary community count: " + modelData.secondary_communities.length
                                            font.pointSize: 12
                                        }


                                    }
                                    Repeater{
                                        model: modelData.secondary_communities
                                        Text{
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            width: parent.width
                                            anchors.leftMargin: 25
                                            visible: drpdown_secondary.selected ? true : false
                                            text: "ID: " + modelData.id + "    " + "Size: " + modelData.pop
                                            font.pointSize: 11//{parental_school_text.pointSize - 2}
                                        }
                                    }
                                    Row{
                                        id: hh_select
                                        height: parent.height / parent.children.length
                                        width: parent.width

                                        Button{
                                            property bool selected: false
                                            width: 15
                                            height: 15
                                            id:drpdown_hh
                                            flat: true
                                            contentItem: Text {
                                                anchors.fill: parent
                                                text:parent.selected ? ">" : "^"
                                                verticalAlignment: Text.AlignVCenter
                                                font.pointSize: 15
                                                //horizontalAlignment: Text.AlignVCenter
                                                //verticalAlignment:  Text.AlignVCenter
                                                color: parent.down ? "white": "green"
                                            }
                                            onClicked: {
                                                selected = !selected

                                                if(selected){
                                                    base_rec.height += 36 * modelData.households.length
                                                }

                                                else{
                                                    base_rec.height -= 36 * modelData.households.length
                                                }
                                            }
                                        }


                                        Text{
                                            text: "Household count: " + modelData.households.length
                                            font.pointSize: 12
                                        }
                                    }
                                    Repeater{
                                        model: modelData.households
                                        Text{
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            width: parent.width
                                            anchors.leftMargin: 25
                                            visible: drpdown_hh.selected ? true : false
                                            text: "ID: " + modelData.id + "    " + "Size: " + modelData.pop
                                            font.pointSize: 11//{parental_school_text.pointSize - 2}
                                        }
                                    }

                                }
                            }

                        }
                    }
                }

            }
        }
    }


    Map {
        id: map
        visible: true
        maximumTilt:0
        plugin: mapPlugin
        center: QtPositioning.coordinate(0, 0)
        zoomLevel: 0
        z:1

        anchors.top: toolBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: sidebar.right
        anchors.right: parent.right

        MapItemView{

            model: backend.cities
            delegate: mapCircleComponent

        }

        MapItemView{
            model: backend.commuters
            delegate: path
        }



        Rectangle {
            id: selectionRect
            visible: false
            x: 0
            y: 0
            z: backend.total_pop  +10
            width: 0
            height: 0
            rotation: 0
            color: "#5F227CEB"
            border.width: 1
            border.color: "#103A6E"
            transformOrigin: Item.TopLeft
            enabled: false
        }

        MouseArea{
            id: selectionMouseArea
            property int initialXPos
            property int initialYPos
            property bool justStarted
            z: 5
            anchors.fill: parent



            onPressed: {
                if (mouse.button === Qt.LeftButton && mouse.modifiers & Qt.ShiftModifier){
                    enabled = true
                    selectionRect.enabled = true
                    selectionRect.x = mouse.x
                    selectionRect.y = mouse.y
                    selectionRect.width = 0
                    selectionRect.height = 0
                    selectionRect.visible = true
                    map.enabled = false
                    justStarted = true
                }
            }
            onPositionChanged: {
                if(selectionRect.visible == true){

                    if (mouse.x < selectionRect.x)
                    {
                        if (mouse.y > selectionRect.y){ // from the right top corner
                            selectionRect.rotation = 90
                            selectionRect.width = mouse.y - selectionRect.y
                            selectionRect.height = selectionRect.x - mouse.x

                        }
                        else{
                            selectionRect.rotation = 180
                            selectionRect.width = selectionRect.x - mouse.x
                            selectionRect.height = selectionRect.y - mouse.y

                        }

                    }
                    else // x is bigger
                    {
                        if (mouse.y >= selectionRect.y){ // from the left top corner
                            selectionRect.rotation = 0
                            selectionRect.width  = mouse.x - selectionRect.x
                            selectionRect.height = mouse.y - selectionRect.y
                        }
                        else{
                            selectionRect.rotation = 270
                            selectionRect.width = selectionRect.y - mouse.y
                            selectionRect.height = mouse.x - selectionRect.x
                        }
                    }
                }

            }
            onReleased: {
                selectionRect.visible = false
                map.enabled = true
                //enabled = false

                var x_low
                var x_high
                var y_low
                var y_high

                if(selectionRect.rotation === 0){
                    x_low  = selectionRect.x
                    x_high = selectionRect.x + selectionRect.width
                    y_low  = selectionRect.y
                    y_high = selectionRect.y + selectionRect.height
                }

                else if(selectionRect.rotation === 90){
                    x_low  = selectionRect.x - selectionRect.height
                    x_high = selectionRect.x
                    y_low  = selectionRect.y
                    y_high = selectionRect.y + selectionRect.width

                }

                else if(selectionRect.rotation === 180){
                    x_low  = selectionRect.x - selectionRect.width
                    x_high = selectionRect.x
                    y_low  = selectionRect.y - selectionRect.height
                    y_high = selectionRect.y

                }

                else{
                    x_low  = selectionRect.x
                    x_high = selectionRect.x + selectionRect.height
                    y_low  = selectionRect.y - selectionRect.width
                    y_high = selectionRect.y
                }

                selectionRect.x = -1
                selectionRect.y = -1
                selectionRect.width = -1
                selectionRect.height = -1

                var items = []
                for(var i = 0; i < map.mapItems.length; i++){
                    var item = map.mapItems[i]
                    if(item.x > x_low && item.x < x_high){
                        if(item.y > y_low && item.y < y_high){
                            if(item.circle === true){
                                items.push(item.city)

                            }
                        }
                    }
                }
                backend.flipItems(items)
            }
        }


        function center_and_zoom(){
            map.zoomLevel = 7.5; // Fixed for now
            map.center = backend.center
        }
    }


    Component{
        id: path
        MapPolyline{
            line.width: modelData.width
            line.color: modelData.color
            path:[modelData.center_city1, modelData.center_city2 ]
        }

    }
    
    Component{
        id: mapCircleComponent
        MapCircle{
            property var city : modelData
            property var perc : modelData.popCount === 0 ? 0 : Math.round(modelData.infected/modelData.popCount * 100)
            property bool circle : true
            radius:  modelData.popCount/ backend.total_pop * 250000
            color: cty_mouse.containsMouse ? Qt.rgba(1, 1, 1, 0.2) : modelData.clicked ? Qt.rgba(1 - perc / 50 ,0,0 + perc /50,0.2 + perc / 50) : Qt.rgba(0,1 - perc / 50,0 + perc / 50 ,0.2 + perc / 50 )
            center: modelData.crd
            z: backend.total_pop - modelData.popCount
            
            MouseArea{
                id: cty_mouse
                anchors.fill: parent
                hoverEnabled: true
                ToolTip.text: "City: " + modelData.name + "\nPopulation: "
                              + modelData.popCount + "\nInfected: " + modelData.infected
                              + "|" + perc +"%"
                ToolTip.visible: containsMouse ? true : false
                
                onClicked: {
                    if(modelData.clicked){
                        modelData.clicked = false
                        //parent.remove_commuters();

                    }
                    else{
                        modelData.clicked = true
                        //parent.draw_commuters()
                    }
                }
                
            }
        }
    }

}
