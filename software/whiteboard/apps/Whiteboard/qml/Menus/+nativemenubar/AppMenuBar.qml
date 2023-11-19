import QtQuick
import Qt.labs.platform
import QtQuick.Controls

import I2Quick


Item {
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
    // Content
    //
    //----------------------------------

    MenuBar {
        Menu {
            title: qsTr("&File")

            MenuItem {
                text: qsTr("Take a snapshot")
                shortcut: StandardKey.AddTab

                onTriggered: {
                    root.takeSnapshot();
                }
            }

            MenuItem {
                text: qsTr("E&xit")

                onTriggered: {
                    root.exit();
                }
            }
        }

        Menu {
            title: qsTr("Debug")

            //enabled: ApplicationUtils.isDebug //FIXME
            visible: enabled

            MenuItemGroup {
                id: debugVisualizationActions
            }

            MenuItem {
                text: qsTr("Normal rendering")
                checkable: true
                checked: true
                group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Normal);
                }
            }

            MenuItem {
                text: qsTr("Visualize clipping")
                checkable: true
                group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Clipping);
                }
            }

            MenuItem {
                text: qsTr("Visualize batches")
                checkable: true
                group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Batches);
                }
            }

            MenuItem {
                text: qsTr("Visualize overdraw")
                checkable: true
                group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Overdraw);
                }
            }

            MenuItem {
                text: qsTr("Visualize changes")
                checkable: true
                group: debugVisualizationActions

                onTriggered: {
                    root.setDebugVisualizationMode(I2QuickDebugVisualizationMode.Changes);
                }
            }
        }
    }
}

