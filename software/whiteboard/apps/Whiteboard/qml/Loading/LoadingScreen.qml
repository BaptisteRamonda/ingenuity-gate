import QtQuick

Item {
    id: root


    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    property color color: "#000000"


    QtObject {
        id: rootPrivate

        readonly property int blockSize: 50
        readonly property int blockRadius: 15
        readonly property int blockSpacing: 5
    }



    //----------------------------------
    //
    // Content
    //
    //----------------------------------

    Item {
        id: content

        anchors.centerIn: parent

        width: childrenRect.width
        height: childrenRect.height


        BlockLoadingAnimation {
            id: letterI

            blockSize: rootPrivate.blockSize
            blockRadius: rootPrivate.blockRadius
            blockSpacing: rootPrivate.blockSpacing

            blockColor: root.color

            visibilitiesOfBlock: [
                false, true, true, true, false,
                false, false, true, false, false,
                false, false, true, false, false,
                false, false, true, false, false,
                false, false, true, false, false,
                false, false, true, false, false,
                false, true, true, true, false
            ]
        }


        BlockLoadingAnimation {
            id: number2

            anchors {
                left: letterI.right
                leftMargin: rootPrivate.blockSize + rootPrivate.blockSpacing
            }

            blockSize: rootPrivate.blockSize
            blockRadius: rootPrivate.blockRadius
            blockSpacing: rootPrivate.blockSpacing

            blockColor: root.color

            visibilitiesOfBlock: [
                false, true,  true,  true,  false,
                true,  false, false, false, true,
                false, false, false, false, true,
                false, false, false, true,  false,
                false, false, true,  false, false,
                false, true,  false, false, false,
                true,  true,  true,  true,  true
            ]
        }


    }
}

