import QtQuick

import I2Quick
import I2Quick.Controls

import Whiteboard


Rectangle {
    id: root

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    property alias text: label.text

    implicitWidth: row.width
    implicitHeight: row.height

    radius: 5

    color: WhiteboardTheme.elementInfoBackgroundColor


    //----------------------------------
    //
    // Signals
    //
    //----------------------------------

    signal clicked();


    //----------------------------------
    //
    // Content
    //
    //----------------------------------

    Row {
        id: row

        height: label.implicitHeight

        Text {
            id: label

            padding: 6

            color: WhiteboardTheme.elementInfoTextColor

            font {
                family: "Asap"
                pixelSize: 11
                bold: true
            }
        }

        I2TextButton {
            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: height

            text: qsTr("X")

            font {
                family: "Asap"
                pixelSize: 11
                bold: true
            }

            defaultBackgroundColor: WhiteboardTheme.elementInfoCloseButtonDefaultBackgroundColor
            hoveredBackgroundColor: WhiteboardTheme.elementInfoCloseButtonHoveredBackgroundColor
            pressedBackgroundColor: WhiteboardTheme.elementInfoCloseButtonPressedBackgroundColor
            disabledBackgroundColor: defaultBackgroundColor

            defaultTextColor: WhiteboardTheme.elementInfoTextColor

            I2Cursor.cursorShape: Qt.PointingHandCursor

            onClicked: {
                root.clicked();
            }
        }
    }
}
