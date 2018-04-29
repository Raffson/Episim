import QtQuick 2.7

Item {
    id: rootDraw

    property int lineWidth: 20
    property color lineColor: "blue"
    property int point1x
    property int point1y
    property int point2x
    property int point2y
    property bool dottedAnimation: false

    PathView {
        id: rooPath
        anchors.fill: parent
        interactive: false
        delegate: Rectangle{ color:lineColor;width:lineWidth;height:lineWidth;radius: lineWidth/2;smooth: true }
        path: Path {
            startX: point1x; startY: point1y
            PathLine {
                x:point2x; y: point2y
            }
        }
    }
}