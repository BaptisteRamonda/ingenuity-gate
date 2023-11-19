import QtQuick
import QtQuick.Controls
import QtQuick.Window

import I2Quick

import Snapshot


Item {
    id: root


    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    QtObject {
        id: rootPrivate

        property color selectedColor: "deepskyblue"
    }



    //----------------------------------
    //
    // Methods
    //
    //----------------------------------

    function clear()
    {
        canvas.clear();
    }


    //----------------------------------
    //
    // Content
    //
    //----------------------------------

    Row {
        id: actions

        height: childrenRect.height

        spacing: 5

        Button {
            id: clearButton

            width: (root.width - actions.spacing * 2)/3

            text: qsTr("Clear")

            onClicked: {
                root.clear();
            }
        }


        Button {
            id: getSnapshotButton

            width: clearButton.width

            text: qsTr("Get snapshot")

            onClicked: {
                Snapshot.getSnapshot();
            }
        }


        Button {
            id: sendSnapshotButton

            width: clearButton.width

            text: qsTr("Send snapshot")

            onClicked: {
                Snapshot.sendImage(drawingArea, Math.random() * image.implicitWidth/2, Math.random() * image.implicitHeight/2, drawingArea.width, drawingArea.height);
            }
        }
    }


    Row {
        id: colorPalette

        anchors {
            top: actions.bottom
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        spacing: 4

        Repeater {
            model: ["deepskyblue", "yellow", "chartreuse", "darkorchid", "orange", "turquoise"]

            delegate: Rectangle {
                id: shape

                property bool isSelected: (rootPrivate.selectedColor === color)

                width: 30
                height: width

                color: modelData

                border {
                    color: "grey"
                    width: isSelected ? 4 : 1
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        rootPrivate.selectedColor = shape.color;
                    }
                }
            }
        }
    }


    Rectangle {
        id: drawingArea

        anchors {
            top: colorPalette.bottom
            topMargin: 10
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        border {
            color: "black"
            width: 1
        }


        Image {
            id: image

            anchors {
                fill: parent
                margins: 1
            }

            source: Snapshot.snapshot

            fillMode: Image.PreserveAspectFit
        }


        Canvas {
            id: canvas

            anchors.fill: parent

            property real lastX
            property real lastY

            function clear()
            {
                let context = getContext("2d");
                context.reset();
                requestPaint();
            }

            onPaint: {
                if (canvasMouseArea.pressed)
                {
                    let context = getContext('2d')
                    context.lineWidth = 4;
                    context.strokeStyle = rootPrivate.selectedColor;
                    context.beginPath();
                    context.moveTo(lastX, lastY);
                    lastX = canvasMouseArea.mouseX;
                    lastY = canvasMouseArea.mouseY;
                    context.lineTo(lastX, lastY);
                    context.stroke();
                }
            }

            MouseArea {
                id: canvasMouseArea

                anchors.fill: parent

                onPressed: {
                    canvas.lastX = mouseX;
                    canvas.lastY = mouseY;
                }

                onPositionChanged: {
                    if (pressed)
                        canvas.requestPaint();
                }
            }
        }
    }
}
