/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtLocation 5.6
import QtQuick.Controls.Material 2.2
import QtPositioning 5.5
import QtQml.Models 2.2
import "map"
import "menus"
import "helper.js" as Helper
import "custom"


ApplicationWindow {
    id: appWindow
    property variant map
    property variant minimap
    property variant parameters
    property variant pop_info
    property var stride_paths: []
    property var infected: 0
    property variant selector_rect: 0
    property variant snap: 0


    readonly property bool inPortrait: appWindow.width < appWindow.height

    header: ToolBar {
        id: overlayHeader
        RowLayout {
            anchors.fill: parent
            Label {
                text: "Stride: simulator for transmission of infectious diseases"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                text: qsTr("update population")
                onClicked: updateSelected()
            }
            ToolButton {
                text: qsTr("show commutings")
                onClicked: showCommutes()
            }
            ToolButton {
                text: qsTr("remove commutes")
                onClicked: removeLines()
            }
            ToolButton {
                text: qsTr("select all")
                onClicked: selectAll()
            }
            ToolButton {
                text: qsTr("deselect all")
                onClicked: deselectAll()
            }
            ToolButton {
                text: qsTr("rectangle select")
                onClicked: selectMultiple()
            }
            ToolButton {
                text: qsTr("⋮")
            }
        }
    }

     Drawer {
        id: drawer
        edge: Qt.RightEdge
        y: overlayHeader.height
        width: appWindow.width / 5
        height: appWindow.height - overlayHeader.height

        modal: inPortrait
        interactive: inPortrait
        position: inPortrait ? 0 : 1
        visible: !inPortrait


        ListView {
            id: listView
            anchors.fill: parent

            headerPositioning: ListView.OverlayHeader
            header: Pane {
                id: header
                z: 2
                width: parent.width

                MenuSeparator {
                    parent: header
                    width: parent.width
                    anchors.verticalCenter: parent.bottom
                    visible: !listView.atYBeginning
                }
            }

            ToolBar {

                ColumnLayout {
                    anchors.fill: parent
                    Label{
                        text: "\nPopulation: " + Number(pop_info)
                        font.pixelSize: 22
                        font.italic: true
                        color: "steelblue"
                    }
                    Label{
                        text: "Infected: " + infected
                        font.pixelSize: 22
                        font.italic: true
                        color: "steelblue"

                    }
                    ToolButton {
                        text: qsTr("update population")
                        onClicked: updateSelected()

                    }
                    ToolButton {
                        text: qsTr("show commuters")
                        onClicked: showCommutes()
                    }

                    Label{
                        text: "\nSelector rectangle"
                        font.pixelSize: 20
                        color: "steelblue"
                    }

                    Label{
                        text: "Width"
                        font.pixelSize: 15
                        font.italic: true
                        color: "steelblue"
                    }
                    ToolButton{
                        text: qsTr("+")
                        onClicked: updateRectanglesSize("increaseWidth")
                    }

                    ToolButton{
                        text: qsTr("-")
                        onClicked: updateRectanglesSize("decreaseWidth")
                    }

                    Label{
                        text: "Height"
                        font.pixelSize: 15
                        font.italic: true
                        color: "steelblue"

                    }
                    ToolButton{
                        text: qsTr("+")
                        onClicked: updateRectanglesSize("increaseHeight")
                    }

                    ToolButton{
                        text: qsTr("-")
                        onClicked: updateRectanglesSize("decreaseHeight")
                    }

                    ToolButton{
                        text: qsTr("Update rectangle")
                        onClicked: updateRectangle()
                    }
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    function selectAll(){
        var circle = Qt.createQmlObject('import "custom"; CityCircle {}', page)
        for (var i = 0; i < map.children.length; i++)
        {
            if(map.children[i].objectName === "mqi"){
                circle = map.children[i].sourceItem
                circle.is_clicked = true
                circle.area_info.font.pointSize = 16
                circle.opacity = 1
            }
        }
        updateSelected()
    }

    function deselectAll(){
        var circle = Qt.createQmlObject('import "custom"; CityCircle {}', page)
        for (var i = 0; i < map.children.length; i++)
        {
            if(map.children[i].objectName === "mqi"){
                circle = map.children[i].sourceItem
                circle.is_clicked = false
                circle.area_info.font.pointSize = 1
                circle.opacity = 0.25
            }
        }
        updateSelected()
    }

    function removeLines(){
        for (var i = 0; i < map.children.length; i++)
        {
            if(map.children[i].objectName === "pD"){
                map.removeMapItem(map.children[i])
                i = 0
            }
        }
    }

    function selectMultiple(){
        if(selector_rect == 0){
            selector_rect = Qt.createQmlObject('import "custom";FlexRectangle {}', page)
        }
        else{
            selector_rect.x = 20
            selector_rect.y = 20
            selector_rect.width = 120
            selector_rect.height = 100

        }

    }

    function updateRectangle(){
        deselectAll()
        for (var i = 0; i < map.children.length; i++)
        {
            if(map.children[i].objectName === "mqi"){
                var circle = map.children[i].sourceItem
                //converting to the coordinate system of the screen
                var screencoor = map.fromCoordinate(QtPositioning.coordinate(circle.lati, circle.longi), true)
                if(isIntersection(screencoor.x, screencoor.y, circle.width/2)){
                    circle.is_clicked = true
                    circle.area_info.font.pointSize = 16
                    circle.opacity = 1
                }

            }
        }
        updateSelected();
    }

    function isIntersection(x, y, width)
    {
        var nearestX = Math.max(selector_rect.x, Math.min(x, selector_rect.x + selector_rect.width))
        var nearestY = Math.max(selector_rect.y, Math.min(y, selector_rect.y + selector_rect.height))
        var deltaX = x - nearestX
        var deltaY = y - nearestY

        return ((deltaX * deltaX + deltaY * deltaY) < (width*width) );
    }

    function updateRectanglesSize(par)
    {
        var scale = 30
        switch(par){
            case "increaseHeight":
                selector_rect.height = selector_rect.height  + scale
                break
            case "increaseWidth":
                selector_rect.width = selector_rect.width + scale
                break
            case "decreaseHeight":
                if( selector_rect.height > 80){
                    selector_rect.height = selector_rect.height - scale
                }
                break
            case "decreaseWidth":
                if( selector_rect.width > 80){
                    selector_rect.width = selector_rect.width - scale
                }
                break

        }
    }

    function updateSelected(){
        var circle = Qt.createQmlObject('import "custom"; CityCircle {}', page)
        var total_count = 0
        infected = 0
        for (var i = 0; i < map.children.length; i++)
        {
            if(map.children[i].objectName === "mqi"){
                circle = map.children[i].sourceItem
                var pop = circle.isSelected().popcount
                total_count += pop
                infected += circle.isSelected().infected

            }
            if(map.children[i].objectName === "pop_info"){
                pop_info = map.children[i]
            }
        }

        pop_info = total_count
    }

    // Draws lines representing the commutings between cities, the factors 5000 and 10000 are arbitrarily chosen to show even small commutes
    function showCommutes(number){
        if(number === undefined) number = 10
        var count = 0;
        for (var i = 0; i < map.children.length; i++){
            if(map.children[i].objectName === "mqi" && count < number){
                count ++;
                var circle = map.children[i];
                var startCoordinate = QtPositioning.coordinate(circle.sourceItem.lati, circle.sourceItem.longi);

                // For the out-commuters
                for (var j = 0; j < number; j++){
                    var out_id = circle.sourceItem.out_commuting[j];
                    var out_size = circle.sourceItem.out_commuting_size[j];
                    for (var k = 0; k < map.children.length; k++){
                        if(map.children[k].objectName === "mqi"){
                            if(map.children[k].sourceItem.city_id === out_id){

                                var endCoordinate = QtPositioning.coordinate(map.children[k].sourceItem.lati, map.children[k].sourceItem.longi);

                                var path = Qt.createQmlObject('import "custom"; PathDraw{}', page);
                                path.addCoordinate(startCoordinate);
                                path.addCoordinate(endCoordinate);

                                path.line.width = (out_size/circle.sourceItem.population)*5000
                                map.addMapItem(path);
                            }
                        }
                    }
                }

                // For the in-commuters
                for (var j = 0; j < number; j++){
                    var in_id = circle.sourceItem.in_commuting[j];
                    var in_size = circle.sourceItem.in_commuting_size[j];
                    for (var k = 0; k < map.children.length; k++){
                        if(map.children[k].objectName === "mqi"){
                            if(map.children[k].sourceItem.city_id === in_id){

                                var endCoordinate = QtPositioning.coordinate(map.children[k].sourceItem.lati, map.children[k].sourceItem.longi);

                                var path = Qt.createQmlObject('import "custom"; PathDraw{}', page);
                                path.addCoordinate(startCoordinate);
                                path.addCoordinate(endCoordinate);

                                path.line.width = (out_size/circle.sourceItem.population)*10000
                                path.line.color = 'blue'
                                map.addMapItem(path);
                            }
                        }
                    }
                }
            }
        }
    }

    //defaults
    //! [routecoordinate]
    property variant fromCoordinate: QtPositioning.coordinate(51.2165845, 4.413545489)
    property variant toCoordinate: QtPositioning.coordinate(51.2165845, 4.413545489)
    //! [routecoordinate]

    function createMap(provider)
    {
        var plugin

        if (parameters && parameters.length>0)
            plugin = Qt.createQmlObject ('import QtLocation 5.6; Plugin{ name:"' + provider + '"; parameters: appWindow.parameters}', appWindow)
        else
            plugin = Qt.createQmlObject ('import QtLocation 5.6; Plugin{ name:"' + provider + '"}', appWindow)

        if (minimap) {
            minimap.destroy()
            minimap = null
        }

        var zoomLevel = null
        var tilt = null
        var bearing = null
        var fov = null
        var center = null
        var panelExpanded = null
        if (map) {
            zoomLevel = map.zoomLevel
            tilt = map.tilt
            bearing = map.bearing
            fov = map.fieldOfView
            center = map.center
            panelExpanded = map.slidersExpanded
            map.destroy()
        }

        map = mapComponent.createObject(page);
        map.plugin = plugin;

        if (zoomLevel != null) {
            map.tilt = tilt
            map.bearing = bearing
            map.fieldOfView = fov
            map.zoomLevel = zoomLevel
            map.center = center
            map.slidersExpanded = panelExpanded
        } else {
            // Use an integer ZL to enable nearest interpolation, if possible.
            map.zoomLevel = Math.floor((map.maximumZoomLevel - map.minimumZoomLevel)/2)
            // defaulting to 45 degrees, if possible.
            map.fieldOfView = Math.min(Math.max(45.0, map.minimumFieldOfView), map.maximumFieldOfView)
        }

        map.forceActiveFocus()
    }

    function setCentre(coords){
        map.center.latitude = coords[0]
        map.center.longitude = coords[1]
        pop_info = Qt.createQmlObject(' import QtQuick 2.7;
                                        Text {
                                           id: info
                                           objectName: "pop_info"
                                           text: ""
                                           color: "green"
                                           font.pointSize: 32
                                           font.bold: true
                                           anchors.top: map.top
                                           anchors.topMargin: 5
                                           anchors.horizontalCenter: map.horizontalCenter
                                        }'
                                       , map)
    }

    function saveToImage(filename){
        console.warn("saving image")
        if (map.grabToImage(function(result) {
                               result.saveToFile(filename);
                                    })){
                                    console.warn("succes")
                                    }
                                    else{
                                    console.warn("fail")
                                    }
    }

    function placeCity(values, commuting_in_id, commuting_in_size, commuting_out_id, commuting_out_size){
        var item = Qt.createQmlObject('import QtQuick 2.7; import QtLocation 5.3; MapQuickItem{objectName: "mqi";}', map, "dynamic")
        item.coordinate = QtPositioning.coordinate(values["latitude"], values["longitude"])

        var circle = Qt.createQmlObject('import "custom"; CityCircle {}', page)
        var wh = 25000*values["perc"]
        var max = 125
        var min = 50
        if (wh > max){
            wh = max
        }
        if (wh < min){
            wh = min
        }
        circle.city_id = values["id"]
        circle.in_commuting = commuting_in_id
        circle.in_commuting_size = commuting_in_size
        circle.out_commuting = commuting_out_id
        circle.out_commuting_size = commuting_out_size
        circle.width = wh
        circle.height = wh
        circle.longi = values["longitude"]
        circle.lati = values["latitude"]
        circle.radius = values["radius"]
        circle.xco = values["x"]
        circle.yco = values["y"]
        circle.opacity = 0.25
        circle.area_text = values["info"]
        circle.infected = values["infected"]
        circle.population = values["population"]
        item.sourceItem = circle
        item.anchorPoint = Qt.point(item.sourceItem.width/2, item.sourceItem.height/2)
        var percentage = circle.infected/circle.population
        circle.color = Qt.hsva(0, 1, percentage, 0.75)

        map.addMapItem(item)
    }

    function getPlugins()
    {
        var plugin = Qt.createQmlObject ('import QtLocation 5.6; Plugin {}', appWindow)
        var myArray = new Array()
        for (var i = 0; i<plugin.availableServiceProviders.length; i++) {
            var tempPlugin = Qt.createQmlObject ('import QtLocation 5.6; Plugin {name: "' + plugin.availableServiceProviders[i]+ '"}', appWindow)
            if (tempPlugin.supportsMapping())
                myArray.push(tempPlugin.name)
        }
        myArray.sort()
        return myArray
    }

    function initializeProviders(pluginParameters)
    {
        var parameters = new Array()
        for (var prop in pluginParameters){
            var parameter = Qt.createQmlObject('import QtLocation 5.6; PluginParameter{ name: "'+ prop + '"; value: "' + pluginParameters[prop]+'"}',appWindow)
            parameters.push(parameter)
        }
        appWindow.parameters = parameters
        var plugins = getPlugins()
        mainMenu.providerMenu.createMenu(plugins)
        for (var i = 0; i<plugins.length; i++) {
            if (plugins[i] === "osm")
                mainMenu.selectProvider(plugins[i])
        }
    }

    title: qsTr("Mapviewer")
    height: 1020
    width: 1680
    visible: true
    menuBar: mainMenu

    //! [geocode0]
    Address {
        id :fromAddress
        street: "Sandakerveien 116"
        city: "Oslo"
        country: "Norway"
        state : ""
        postalCode: "0484"
    }
    //! [geocode0]

    Address {
        id: toAddress
        street: "Holmenkollveien 140"
        city: "Oslo"
        country: "Norway"
        postalCode: "0791"
    }

    MainMenu {
        id: mainMenu


        onSelectProvider: {
            stackView.pop()
            for (var i = 0; i < providerMenu.items.length; i++) {
                providerMenu.items[i].checked = providerMenu.items[i].text === providerName
            }

            createMap(providerName)
            if (map.error === Map.NoError) {
                selectMapType(map.activeMapType)
                toolsMenu.createMenu(map);
            } else {
                mapTypeMenu.clear();
                toolsMenu.clear();
            }
        }

        onSelectMapType: {
            stackView.pop(page)
            for (var i = 0; i < mapTypeMenu.items.length; i++) {
                mapTypeMenu.items[i].checked = mapTypeMenu.items[i].text === mapType.name
            }
            map.activeMapType = mapType
        }


        onSelectTool: {
            switch (tool) {
            case "AddressRoute":
                stackView.pop({item:page, immediate: true})
                stackView.push({ item: Qt.resolvedUrl("forms/RouteAddress.qml") ,
                                   properties: { "plugin": map.plugin,
                                       "toAddress": toAddress,
                                       "fromAddress": fromAddress}})
                stackView.currentItem.showRoute.connect(map.calculateCoordinateRoute)
                stackView.currentItem.showMessage.connect(stackView.showMessage)
                stackView.currentItem.closeForm.connect(stackView.closeForm)
                break
            case "CoordinateRoute":
                stackView.pop({item:page, immediate: true})
                stackView.push({ item: Qt.resolvedUrl("forms/RouteCoordinate.qml") ,
                                   properties: { "toCoordinate": toCoordinate,
                                       "fromCoordinate": fromCoordinate}})
                stackView.currentItem.showRoute.connect(map.calculateCoordinateRoute)
                stackView.currentItem.closeForm.connect(stackView.closeForm)
                break

            case "Clear":
                map.clearData()
                break

            default:
                console.log("Unsupported operation")
            }
        }


    }

    MapPopupMenu {
        id: mapPopupMenu

        function show(coordinate)
        {
            stackView.pop(page)
            mapPopupMenu.coordinate = coordinate
            mapPopupMenu.markersCount = map.markers.length
            mapPopupMenu.mapItemsCount = map.mapItems.length
            mapPopupMenu.update()
            mapPopupMenu.popup()
        }

        onItemClicked: {
            stackView.pop(page)
            switch (item) {
            case "addMarker":
                map.addMarker()
                break
            case "getCoordinate":
                map.coordinatesCaptured(coordinate.latitude, coordinate.longitude)
                break
            case "fitViewport":
                map.fitViewportToMapItems()
                break
            case "deleteMarkers":
                map.deleteMarkers()
                break
            case "deleteItems":
                map.deleteMapItems()
                break
            default:
                console.log("Unsupported operation")
            }
        }
    }

    MarkerPopupMenu {
        id: markerPopupMenu

        function show(coordinate)
        {
            stackView.pop(page)
            markerPopupMenu.markersCount = map.markers.length
            markerPopupMenu.update()
            markerPopupMenu.popup()
        }

        function askForCoordinate()
        {
            stackView.push({ item: Qt.resolvedUrl("forms/ReverseGeocode.qml") ,
                               properties: { "title": qsTr("New Coordinate"),
                                   "coordinate":   map.markers[map.currentMarker].coordinate}})
            stackView.currentItem.showPlace.connect(moveMarker)
            stackView.currentItem.closeForm.connect(stackView.closeForm)
        }

        function moveMarker(coordinate)
        {
            map.markers[map.currentMarker].coordinate = coordinate;
            map.center = coordinate;
            stackView.pop(page)
        }

        onItemClicked: {
            stackView.pop(page)
            switch (item) {
            case "deleteMarker":
                map.deleteMarker(map.currentMarker)
                break;
            case "getMarkerCoordinate":
                map.coordinatesCaptured(map.markers[map.currentMarker].coordinate.latitude, map.markers[map.currentMarker].coordinate.longitude)
                break;
            case "moveMarkerTo":
                askForCoordinate()
                break;
            case "routeToNextPoint":
            case "routeToNextPoints":
                map.calculateMarkerRoute()
                break
            case "distanceToNextPoint":
                var coordinate1 = map.markers[currentMarker].coordinate;
                var coordinate2 = map.markers[currentMarker+1].coordinate;
                var distance = Helper.formatDistance(coordinate1.distanceTo(coordinate2));
                stackView.showMessage(qsTr("Distance"),"<b>" + qsTr("Distance:") + "</b> " + distance)
                break
            case "drawImage":
                map.addGeoItem("ImageItem")
                break
            case "drawRectangle":
                map.addGeoItem("RectangleItem")
                break
            case "drawCircle":
                map.addGeoItem("CircleItem")
                break;
            case "drawPolyline":
                map.addGeoItem("PolylineItem")
                break;
            case "drawPolygonMenu":
                map.addGeoItem("PolygonItem")
                break
            default:
                console.log("Unsupported operation")
            }
        }
    }

    ItemPopupMenu {
        id: itemPopupMenu

        function show(type,coordinate)
        {
            stackView.pop(page)
            itemPopupMenu.type = type
            itemPopupMenu.update()
            itemPopupMenu.popup()
        }

        onItemClicked: {
            stackView.pop(page)
            switch (item) {
            case "showRouteInfo":
                stackView.showRouteListPage()
                break;
            case "deleteRoute":
                map.routeModel.reset();
                break;
            case "showPointInfo":
                map.showGeocodeInfo()
                break;
            case "deletePoint":
                map.geocodeModel.reset()
                break;
            default:
                console.log("Unsupported operation")
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        focus: true
        initialItem: Item {
            id: page
        }

        function showMessage(title,message,backPage)
        {
            push({ item: Qt.resolvedUrl("forms/Message.qml") ,
                               properties: {
                                   "title" : title,
                                   "message" : message,
                                   "backPage" : backPage
                               }})
            currentItem.closeForm.connect(closeMessage)
        }

        function closeMessage(backPage)
        {
            pop(backPage)
        }

        function closeForm()
        {
            pop(page)
        }

        function showRouteListPage()
        {
            push({ item: Qt.resolvedUrl("forms/RouteList.qml") ,
                               properties: {
                                   "routeModel" : map.routeModel
                               }})
            currentItem.closeForm.connect(closeForm)
        }
    }

    Component {
        id: mapComponent

        MapComponent{
            width: page.width
            height: page.height
            onFollowmeChanged: mainMenu.isFollowMe = map.followme
            onSupportedMapTypesChanged: mainMenu.mapTypeMenu.createMenu(map)
            onCoordinatesCaptured: {
                var text = "<b>" + qsTr("Latitude:") + "</b> " + Helper.roundNumber(latitude,4) + "<br/><b>" + qsTr("Longitude:") + "</b> " + Helper.roundNumber(longitude,4)
                stackView.showMessage(qsTr("Coordinates"),text);
            }
            onGeocodeFinished:{
                if (map.geocodeModel.status == GeocodeModel.Ready) {
                    if (map.geocodeModel.count == 0) {
                        stackView.showMessage(qsTr("Geocode Error"),qsTr("Unsuccessful geocode"))
                    } else if (map.geocodeModel.count > 1) {
                        stackView.showMessage(qsTr("Ambiguous geocode"), map.geocodeModel.count + " " +
                                              qsTr("results found for the given address, please specify location"))
                    } else {
                        stackView.showMessage(qsTr("Location"), geocodeMessage(),page)
                    }
                } else if (map.geocodeModel.status == GeocodeModel.Error) {
                    stackView.showMessage(qsTr("Geocode Error"),qsTr("Unsuccessful geocode"))
                }
            }
            onRouteError: stackView.showMessage(qsTr("Route Error"),qsTr("Unable to find a route for the given points"),page)

            onShowGeocodeInfo: stackView.showMessage(qsTr("Location"),geocodeMessage(),page)

            onErrorChanged: {
                if (map.error != Map.NoError) {
                    var title = qsTr("ProviderError")
                    var message =  map.errorString + "<br/><br/><b>" + qsTr("Try to select other provider") + "</b>"
                    if (map.error == Map.MissingRequiredParameterError)
                        message += "<br/>" + qsTr("or see") + " \'mapviewer --help\' "
                                + qsTr("how to pass plugin parameters.")
                    stackView.showMessage(title,message);
                }
            }
            onShowMainMenu: mapPopupMenu.show(coordinate)
            onShowMarkerMenu: markerPopupMenu.show(coordinate)
            onShowRouteMenu: itemPopupMenu.show("Route",coordinate)
            onShowPointMenu: itemPopupMenu.show("Point",coordinate)
            onShowRouteList: stackView.showRouteListPage()
        }
    }
}
