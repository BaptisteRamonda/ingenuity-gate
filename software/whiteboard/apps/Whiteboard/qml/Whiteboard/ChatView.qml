import QtQuick
import QtQuick.Controls.Basic

import Whiteboard


Rectangle {
    id: root


    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    implicitWidth: 400

    color: WhiteboardTheme.chatViewBackgroundColor


    QtObject {
        id: rootPrivate

        readonly property int chatMessageVerticalPadding: 5
    }



    //----------------------------------
    //
    // Content
    //
    //----------------------------------

    Text {
        anchors.fill: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: qsTr("No message")

        color: WhiteboardTheme.chatViewMessageTextColor

        font {
            family: "Asap"
            pixelSize: 24
            bold: true
        }

        visible: (listView.count === 0)
    }


    ListView {
        id: listView

        anchors.fill: parent

        ScrollBar.vertical: ScrollBar {
        }

        model: Whiteboard.messages

        delegate: Item {
            id: item

            width: ListView.view.width
            height: rootPrivate.chatMessageVerticalPadding * 2 + (fromChatBot ? Math.max(chatIcon.height, infoAndMessageBubble.height) : infoAndMessageBubble.height)

            property Message message: model.qtObject
            property real fromChatBot: (message?.fromChatBot ?? false)

            Item {
                id: chatIcon

                anchors {
                    top: parent.top
                    topMargin: rootPrivate.chatMessageVerticalPadding
                }

                width: 40
                height: 30

                visible: item.fromChatBot

                Image {
                    anchors.centerIn: parent

                    source: "qrc:/resources/images/chat-icon.svg"

                    sourceSize: Qt.size(30, 30)
                }
            }

            Column {
                id: infoAndMessageBubble

                anchors {
                    top: parent.top
                    topMargin: rootPrivate.chatMessageVerticalPadding
                    left: parent.left
                    leftMargin: item.fromChatBot ? 40 : 80
                    right: parent.right
                    rightMargin: item.fromChatBot ? 50 : 10
                }

                spacing: 3

                Text {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    horizontalAlignment: (item.fromChatBot ? Text.AlignLeft : Text.AlignRight)

                    color: WhiteboardTheme.chatViewMessageTextColor

                    elide: Text.ElideRight

                    text: item.fromChatBot
                          ? (item.message?.chatBotName ?? "")
                          : qsTr("Whiteboard")

                    font {
                        family: "Asap"
                        pixelSize: 14
                        bold: true
                    }
                }

                Rectangle {
                    id: messageBubble

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: childrenRect.height

                    radius: 5

                    color: WhiteboardTheme.chatViewMessageBackgroundColor

                    Text {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        padding: 5

                        color: WhiteboardTheme.chatViewMessageTextColor

                        wrapMode: Text.WordWrap

                        text: (item.message?.text ?? "")

                        font {
                            family: "Asap"
                            pixelSize: 12
                        }
                    }
                }
            }
        }

        onCountChanged: {
            Qt.callLater(() => {
                listView.positionViewAtEnd();
            });
        }
    }
}
