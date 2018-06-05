import QtQuick 2.0
import QtLocation 5.6

Rectangle {
    id: flexRectangle
    x:200; y:100; width: 120; height: 100
    border.width: 3
    color: "#354682B4"

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        drag.target: flexRectangle
        drag.axis: Drag.XAndYAxis
        drag.minimumX: 0
        drag.minimumY: 0

    }

}