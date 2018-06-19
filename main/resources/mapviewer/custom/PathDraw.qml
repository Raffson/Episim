import QtLocation 5.3
import QtQuick 2.7
import QtQml 2.2

// Line that represents a commute.

MapPolyline {
    id: pathDraw
    objectName: "pD"
    line.width: 3
    line.color: 'green'
    property double longi1: 0
    property double longi2: 0
    property double lati1:  0
    property double lati2: 0
    path: []
}
