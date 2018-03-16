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

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.5
import "map"
import "menus"
import "helper.js" as Helper

ApplicationWindow {
    id: appWindow
    property variant map
    property variant minimap
    property variant parameters

    //defaults
    //! [routecoordinate]
    property variant fromCoordinate: QtPositioning.coordinate(59.9483, 10.7695)
    property variant toCoordinate: QtPositioning.coordinate(59.9645, 10.671)
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
    height: 640
    width: 360
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


    }

    StackView {
        id: stackView
        anchors.fill: parent
        focus: true
        initialItem: Item {
            id: page
        }
    }

    Component {
        id: mapComponent

        MapComponent{
            width: page.width
            height: page.height
            onShowMainMenu: mapPopupMenu.show(coordinate)
        }
    }
}
