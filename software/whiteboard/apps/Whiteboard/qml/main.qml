import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Qt.labs.platform as Labs

import I2Quick

import Whiteboard
import "Loading" as Loading
import "Menus" as Menus


ApplicationWindow {
    id: root

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    title: qsTr("%1").arg(Qt.application.displayName)

    width: 1080
    height: 540
    minimumWidth: 800
    minimumHeight: 600

    color: WhiteboardTheme.applicationWindowColor

    visible: true


    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    menuBar: Menus.AppMenuBar {
        onTakeSnapshot: {
            root.takeSnapshot();
        }

        onExit: {
            Qt.quit();
        }


        onSetDebugVisualizationMode: (value) => {
            root.I2QuickDebug.visualizationMode = value;
        }
    }


    //----------------------------------
    //
    // Methods
    //
    //----------------------------------

    function takeSnapshot()
    {
        takeSnapshotWithCustomPixelRatio(Screen.devicePixelRatio);
    }


    function takeSnapshotWithCustomPixelRatio(pixelRatio)
    {
        // NB: we must show our snapshotBackground rectangle
        //     to avoid empty transparent areas (png) in our snapshot
        snapshotBackground.visible = true;

        // NB: we must use 'content.Window.contentItem' instead of 'content'
        //     because the Popup items are attached to ApplicationWindow.overlay
        //     that is a sibling of ApplicationWindow.contentItem.
        //     Both of them are attached to Window.contentItem
        content.Window.contentItem.grabToImage(function(result) {
            snapshotBackground.visible = false;

            I2SnapshotHelper.saveItemGrabResult(result, Qt.application.name, "-yyyy-MM-dd--hh-mm-ss-zzz" + ((pixelRatio > 1) ? ".@" + pixelRatio + "x" : ""));
        }, Qt.size(content.width * pixelRatio, content.height * pixelRatio));
    }


    //----------------------------------
    //
    // Behaviors
    //
    //----------------------------------

    Component.onCompleted: {
        I2SnapshotHelper.directory = I2Utils.urlToLocalFile(Labs.StandardPaths.writableLocation(Labs.StandardPaths.DocumentsLocation) + "/IngeScape/snapshots");

        loaderAnimation.start();
        Whiteboard.qmlItem = content;
    }


    //----------------------------------
    //
    // Content
    //
    //----------------------------------


    Item {
        id: content

        anchors.fill: parent


        Rectangle {
            id: snapshotBackground

            anchors.fill: parent

            visible: false
            color: root.color
        }


        Loading.LoadingScreen {
            id: loadingScreen

            anchors.fill: parent

            visible: (opacity > 0)
            opacity: ((applicationLoader.status !== Loader.Ready) && (applicationLoader.status !== Loader.Error)) ? 1 : 0

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
        }


        Text {
            id: errorMessage

            anchors.fill: parent

            visible: (applicationLoader.status === Loader.Error)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("QML fatal error")
            color: "#E10600"

            font {
                bold: true
                pixelSize: 72
            }
        }


        Loader {
            id: applicationLoader

            anchors.fill: parent

            asynchronous: !I2QmlHotReloader.isRunning
            visible: (opacity > 0)
            opacity: (status === Loader.Ready) ? 1 : 0

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
        }


        PauseAnimation {
            id: loaderAnimation

            duration: 250

            onStopped: {
                applicationLoader.source = Qt.binding(function() { return I2QmlHotReloader.url; });
            }
        }
    }
}

