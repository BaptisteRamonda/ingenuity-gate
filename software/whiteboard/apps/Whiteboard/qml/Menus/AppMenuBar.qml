import QtQuick
import QtQuick.Window
import QtQuick.Controls

import I2Quick


MenuBar {
    id: root

    //----------------------------------
    //
    // Signals
    //
    //----------------------------------

    signal takeSnapshot();
    signal takeSnapshotOfWholeView();
    signal exit();

    signal setDebugVisualizationMode(int value);


    //----------------------------------
    //
    // Behaviors
    //
    //----------------------------------

    Component.onCompleted: {
        //if (ApplicationUtils.isDebug)
            addMenu(componentDebugMenu.createObject(root));
    }


    //----------------------------------
    //
    // Content
    //
    //----------------------------------

    Menu {
        title: qsTr("&File")

        Action {
            text: qsTr("Take a snapshot")
            shortcut: StandardKey.AddTab

            onTriggered: {
                root.takeSnapshot();
            }
        }

        Action {
            text: qsTr("E&xit")

            onTriggered: {
                root.exit();
            }
        }
    }

    Component {
       id: componentDebugMenu

        Menu {
            title: qsTr("Debug")

            ActionGroup {
                id: debugVisualizationActions
            }

            Action {
                text: qsTr("Normal rendering")
                checkable: true
                checked: true
                ActionGroup.group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Normal);
                }
            }

            Action {
                text: qsTr("Visualize clipping")
                checkable: true
                ActionGroup.group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Clipping);
                }
            }

            Action {
                text: qsTr("Visualize batches")
                checkable: true
                ActionGroup.group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Batches);
                }
            }

            Action {
                text: qsTr("Visualize overdraw")
                checkable: true
                ActionGroup.group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Overdraw);
                }
            }

            Action {
                text: qsTr("Visualize changes")
                checkable: true
                ActionGroup.group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Changes);
                }
            }
        }
    }
}
