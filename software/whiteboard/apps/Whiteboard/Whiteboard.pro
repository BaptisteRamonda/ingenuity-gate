#####################################################################
#
# Generic (multi-platform) rules
#
#####################################################################

VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_BUILD = 0

TARGET = Whiteboard
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

# To get the version in our C++ code
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "APP_NAME=\\\"$$TARGET\\\""

QT += widgets qml quick quick-private quickcontrols2impl-private svg core-private testlib concurrent

CONFIG += c++17 precompile_header
PRECOMPILED_HEADER = stable.h

# To build temp files (*.o, moc_*.cpp and qrc_*.cpp) in a temp dir
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp
QMLCACHE_DIR = tmp


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Project sources, headers and resources
SOURCES += \
    main.cpp \
    models/graphicelement.cpp \
    models/message.cpp \
    qmlinstrument.cpp


HEADERS += \
    models/graphicelement.h \
    models/message.h \
    qmlinstrument.h


RESOURCES += \
    qml.qrc


OTHER_FILES += \



# Additional include paths
INCLUDEPATH += $$PWD/../../internal/I2Quick/src/I2Quick

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD/qml $$PWD/../../internal/I2Quick/src/I2Quick/qml

# Embed I2Quick into our library
DEFINES += I2QUICK_BUILD_LIBRARY
!include($$PWD/../../internal/I2Quick/src/I2Quick/I2Quick.pri) {
    error(Could not load I2Quick.pri)
}


# Include agents
!include($$PWD/agents/agents.pri) {
    error(Could not load agents.pri)
}


CONFIG(debug, debug|release) {
    win32 {
        DEFINES += INGESCAPE
    }

    # Additional include paths
    INCLUDEPATH += $$PWD/../../../../ingescape/include
    INCLUDEPATH += $$PWD/../../../../ingescape/dependencies/czmq/include
    INCLUDEPATH += $$PWD/../../../../ingescape/dependencies/libzmq/include
    INCLUDEPATH += $$PWD/../../../../ingescape/dependencies/zyre/include

    # Include ingescape library
    !include($$PWD/../../../../ingescape/builds/qt/ingescape.pri) {
        error(Could not load ingescape.pri)
    }
}



#####################################################################
#
# Platform specific rules
#
#####################################################################
mac {
    message($${TARGET} - macOS specific rules)

    QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64

    CONFIG(release, debug|release) {
        INCLUDEPATH += /usr/local/include

        LIBS += /usr/local/lib/libczmq.dylib
        LIBS += /usr/local/lib/libzyre.dylib
        LIBS += /usr/local/lib/libsodium.dylib
        LIBS += /usr/local/lib/libingescape.dylib
        LIBS += -framework CoreFoundation

        librariesToCopy.files += /usr/local/lib/libzmq.5.dylib
        librariesToCopy.files += /usr/local/lib/libczmq.4.dylib
        librariesToCopy.files += /usr/local/lib/libzyre.2.dylib
        librariesToCopy.files += /usr/local/lib/libsodium.23.dylib
        librariesToCopy.files += /usr/local/lib/libingescape.3.0.0.dylib

        QMAKE_POST_LINK += $$quote(cd `dirname $(QMAKE)` && macdeployqt $${OUT_PWD}/$${TARGET}.app -qmldir=$${PWD} $$escape_expand(\n\t))

        QMAKE_BUNDLE_DATA += librariesToCopy
    }

    ICON = icon.icns
}


win32 {
    message($${TARGET} - Windows specific rules)

    CONFIG(release, debug|release) {
        INCLUDEPATH += $$(ProgramFiles)/ingescape/include
        LIBS += -L$$(ProgramFiles)/ingescape/lib/ -lzyre -lczmq -llibsodium -lingescape

        QMAKE_POST_LINK += $$shell_quote($$shell_path($$[QT_INSTALL_BINS]/windeployqt)) $${OUT_PWD}/$${TARGET}.exe -xml -concurrent -printsupport -qmldir=$${PWD} $$escape_expand(\n\t)
    }

    LIBS += -lWtsapi32 -luser32 -lshell32 -lws2_32

    librariesToCopy.path = $${OUT_PWD}
    INSTALLS += librariesToCopy

    RC_ICONS = icon.ico

    QMAKE_TARGET_PRODUCT = $${TARGET}
    QMAKE_TARGET_COPYRIGHT = Copyright © 2022-2023 Ingenuity i/o. All rights reserved.
}

