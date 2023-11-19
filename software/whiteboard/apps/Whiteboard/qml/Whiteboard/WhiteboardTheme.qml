pragma Singleton

import QtQuick

QtObject {
    id: root

    //-----------------------------------------
    //
    // Colors
    //
    //-----------------------------------------


    readonly property color applicationWindowColor: "#FFFFFF"

    readonly property color titleColor: "#000000"

    readonly property color elementInfoBackgroundColor: "#CC373940"
    readonly property color elementInfoTextColor: "#FFFFFF"

    readonly property color elementInfoCloseButtonDefaultBackgroundColor: "transparent"
    readonly property color elementInfoCloseButtonHoveredBackgroundColor:"#212431"
    readonly property color elementInfoCloseButtonPressedBackgroundColor: "#121212"

    readonly property color chatViewBackgroundColor: "#373940"
    readonly property color chatViewMessageBackgroundColor: "#4E5156"
    readonly property color chatViewMessageTextColor: "#FFFFFF"


    //-----------------------------------------
    //
    // Fonts
    //
    //-----------------------------------------


    readonly property FontLoader fontLoaderAsapBold: FontLoader {
      source: "qrc:/resources/fonts/asap/Asap-Bold.ttf"
    }


    readonly property FontLoader fontLoaderAsapRegular: FontLoader {
      source: "qrc:/resources/fonts/asap/Asap-Regular.ttf"
    }
}
