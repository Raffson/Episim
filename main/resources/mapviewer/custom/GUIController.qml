import QtQuick 2.4

Item {
    id: guiControler
    width: 900
    height: 800

    Image {
        id: backGround
        width: 900
        height: 800
        opacity: 0.6
        source: "../../../../../Desktop/ualogo.png"
    }

    Column {
        id: column
        x: 681
        y: 0
        width: 220
        height: 800
        spacing: 5
        anchors.right: parent.right
        anchors.rightMargin: 0

        Rectangle {
            id: openConfig
            y: 0
            width: 200
            height: 110
            color: "#0d5c72"
            border.width: 5
            opacity: 0.7

            MouseArea {
                id: configFile
                x: 0
                y: 0
                width: 220
                height: 110
                anchors.right: parent.right
                anchors.rightMargin: -20

                Text {
                    id: configText
                    x: 0
                    y: 22
                    width: 220
                    height: 65
                    text: qsTr("Open a configuration file")
                    textFormat: Text.PlainText
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                }
            }
        }

        Rectangle {
            id: runSim
            width: 200
            height: 110
            color: "#0d5c72"
            border.width: 5
            opacity: 0.7

            MouseArea {
                id: singleRun
                x: 0
                y: 0
                width: 220
                height: 110
                anchors.right: parent.right
                anchors.rightMargin: -20
                Text {
                    id: singleText
                    x: 0
                    y: 22
                    width: 220
                    height: 65
                    text: qsTr("Run the simulator")
                    textFormat: Text.PlainText
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Rectangle {
            id: oneStep
            width: 200
            height: 110
            color: "#0d5c72"
            border.width: 5
            opacity: 0.7

            MouseArea {
                id: singleStep
                x: 0
                y: 0
                width: 220
                height: 110
                anchors.right: parent.right
                anchors.rightMargin: -20
                Text {
                    id: stepText
                    x: 0
                    y: 22
                    width: 220
                    height: 65
                    text: qsTr("Run one step")
                    textFormat: Text.PlainText
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Rectangle {
            id: multipleSteps
            y: 330
            width: 200
            height: 110
            color: "#0d5c72"
            border.width: 5
            opacity: 0.7

            MouseArea {
                id: mulitStep
                x: 0
                y: 0
                width: 220
                height: 110
                anchors.right: parent.right
                anchors.rightMargin: -20
                Text {
                    id: multiText
                    x: 0
                    y: 22
                    width: 220
                    height: 65
                    text: qsTr("Run multiple steps")
                    textFormat: Text.PlainText
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    Rectangle {
        id: save
        x: 310
        y: 480
        width: 80
        height: 40
        color: "#09c82a"
        opacity: 0.7

        MouseArea {
            id: saveButton
            x: 0
            y: 0
            width: 80
            height: 40

            Text {
                id: saveText
                width: 80
                height: 40
                text: qsTr("Save file")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 12
            }
        }
    }

    Rectangle {
        id: rectangle
        x: 14
        y: 15
        width: 660
        height: 460
        color: "#00000000"
        border.width: 5

        TextEdit {
            id: textEdit
            x: 6
            y: 8
            width: 644
            height: 444
            text: qsTr("Config file will be loaded here for editing")
            anchors.left: parent.left
            anchors.leftMargin: 8
            font.letterSpacing: 1
            font.wordSpacing: 2
            cursorVisible: true
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }
    }
}
