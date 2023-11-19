import QtQuick



/**
 * Inspired by https://dribbble.com/shots/3250272-Animated-Loader-Principle-Freebie
 */
Item {
    id: rootItem


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    property int horizontalBlockCount: 5
    property int verticalBlockCount: 7

    property int animationStepDurationInMilliseconds: 200

    property int blockSize: 50
    property int blockRadius: 3
    property int blockSpacing: 10

    property color blockColor: "white"

    // Visibility flags of blocks column by colum (top-down, left-right)
    // NB: the array must have verticalBlockCount * horizontalBlockCount values
    property var visibilitiesOfBlock: [
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true
                                      ]



    implicitWidth: (horizontalBlockCount + 1) * blockSize + horizontalBlockCount * blockSpacing
    implicitHeight: (verticalBlockCount + 1) * blockSize + verticalBlockCount * blockSpacing



    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Row {
        spacing: rootItem.blockSpacing

        Repeater {
            id: rowRepeater

            model: rootItem.horizontalBlockCount

            delegate: Column {
                id: columnItem

                width: rootItem.blockSize
                spacing: rootItem.blockSpacing

                readonly property int _columnIndex: index
                readonly property int _maxMinusIndex: rootItem.horizontalBlockCount - 1  - index


                Repeater {
                    model: rootItem.verticalBlockCount

                    delegate: Item {
                        id: block

                        readonly property int _blockIndex: columnItem._columnIndex + index * rootItem.horizontalBlockCount

                        width: rootItem.blockSize
                        height: rootItem.blockSize

                        transformOrigin: Item.BottomRight

                        Rectangle {
                            id: rect

                            anchors.fill: parent

                            radius: rootItem.blockRadius

                            color: rootItem.blockColor

                            smooth: true

                            visible: (rootItem.visibilitiesOfBlock.length - 1 >= _blockIndex) ? rootItem.visibilitiesOfBlock[_blockIndex] : false
                        }

                        SequentialAnimation {
                            loops: Animation.Infinite
                            running: rect.visible

                            RotationAnimator {
                                target: block
                                duration: columnItem._maxMinusIndex * rootItem.animationStepDurationInMilliseconds
                                from: 0
                                to: 0
                            }

                            RotationAnimator {
                                target: block
                                duration: rootItem.animationStepDurationInMilliseconds * 4
                                from: 0
                                to: 90
                                easing.type: Easing.InOutQuint
                            }

                            RotationAnimator {
                                target: block
                                duration: columnItem._columnIndex * rootItem.animationStepDurationInMilliseconds * 2
                                from: 90
                                to: 90
                            }

                            RotationAnimator {
                                target: block
                                duration: rootItem.animationStepDurationInMilliseconds * 4
                                from: 90
                                to: 0
                                easing.type: Easing.InOutQuint
                            }

                            RotationAnimator {
                                target: block
                                duration: columnItem._maxMinusIndex * rootItem.animationStepDurationInMilliseconds
                                from: 0
                                to: 0
                            }
                        }
                    }
                }
            }
        }
    }
}

