import QtLocation 5.3

MapPolyline {
        id: pathDraw
        line.width: 3
        line.color: 'green'
        property double longi1: 0
        property double longi2: 0
        property double lati1:  0
        property double lati2: 0
        path: [
            { latitude: lati1, longitude: longi1 },
            { latitude: lati2, longitude: longi2 }
        ]
    }
