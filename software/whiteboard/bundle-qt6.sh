#!/bin/bash

set -e

function usage {
    echo "Usage: ./bundle-qt6-app.sh <package_name> <package_version> <app_install_dir> <path_to_binary> <path_to_qmake>"
    echo "Example: ./bundle-qt6-app.sh whiteboard 1.0.0 Whiteboard /path/to/Whiteboard /usr/local/Qt-6.2.4/bin/qmake"
    echo "  will generate whiteboard-1.0.0_0-amd64.deb that will install Whiteboard under /opt/Whiteboard"
}
SCRIPT_DIR=$(cd `dirname $0`; pwd)
PACKAGE_NAME=$1
PACKAGE_VERSION=$2
APP_INSTALL_DIR=$3
PATH_TO_BINARY=$4
PATH_TO_QMAKE=$5

if [[ -z $PACKAGE_NAME ]]
then
    echo "Package name missing"
    usage
    exit 10
fi

if [[ -z $PACKAGE_VERSION ]]
then
    echo "Package version missing"
    usage
    exit 20
fi

if [[ -z $APP_INSTALL_DIR ]]
then
    echo "App install dir missing"
    usage
    exit 30
fi

if [[ -z $PATH_TO_BINARY ]]
then
    echo "Path to binary missing"
    usage
    exit 40
fi

if [[ -z $PATH_TO_QMAKE ]]
then
    echo "Path to qmake missing"
    usage
    exit 50
fi

$PATH_TO_QMAKE --version

rm -rf $SCRIPT_DIR/DistributionKit

cqtdeployer -bin $PATH_TO_BINARY -qmlDir ${SCRIPT_DIR}/apps/$APP_INSTALL_DIR/qml -qmake $PATH_TO_QMAKE -libDir /usr/lib/x86_64-linux-gnu/ noTranslations
cqtdeployer -bin $PATH_TO_BINARY -qmlDir ${SCRIPT_DIR}/internal/I2Quick/src/I2Quick/qml -qmake $PATH_TO_QMAKE -libDir /usr/lib/x86_64-linux-gnu/ noTranslations

for l in $(find /usr/lib -name "libicu*")
do
  cp -pv $l $SCRIPT_DIR/DistributionKit/lib
done

PACKAGE_DIR=$SCRIPT_DIR/$PACKAGE_NAME-${PACKAGE_VERSION}_0-amd64
rm -rf $PACKAGE_DIR
mkdir -p $PACKAGE_DIR/opt
mv $SCRIPT_DIR/DistributionKit $PACKAGE_DIR/opt/$APP_INSTALL_DIR

#NOTE For the OpenGL backend to work on ubuntu 20.04+, we need to remove some embedded libs from the packaging.
rm $PACKAGE_DIR/opt/$APP_INSTALL_DIR/lib/libstdc++.so*
rm $PACKAGE_DIR/opt/$APP_INSTALL_DIR/lib/libdrm.so*

# Embbed ingescape whithin the installation package
cp $ARTIFACTS_NET_SHARE/linux64/binaries/ingescape/lib/libsodium.so.23 $PACKAGE_DIR/opt/$APP_INSTALL_DIR/lib
cp $ARTIFACTS_NET_SHARE/linux64/binaries/ingescape/lib/libzmq.so.5 $PACKAGE_DIR/opt/$APP_INSTALL_DIR/lib
cp $ARTIFACTS_NET_SHARE/linux64/binaries/ingescape/lib/libczmq.so.4 $PACKAGE_DIR/opt/$APP_INSTALL_DIR/lib
cp $ARTIFACTS_NET_SHARE/linux64/binaries/ingescape/lib/libzyre.so.2 $PACKAGE_DIR/opt/$APP_INSTALL_DIR/lib
cp $ARTIFACTS_NET_SHARE/linux64/binaries/ingescape/lib/libingescape.so.3.0.0 $PACKAGE_DIR/opt/$APP_INSTALL_DIR/lib

mkdir $PACKAGE_DIR/DEBIAN
cat > $PACKAGE_DIR/DEBIAN/control <<EOF
Package: $PACKAGE_NAME
Version: $PACKAGE_VERSION
Architecture: amd64
Maintainer: Ingenuity I/O (dev@ingenuity.io)
Description: $APP_INSTALL_DIR
Category: Utility
EOF

(
  cd $PACKAGE_DIR
  dpkg-deb --build --root-owner-group $PACKAGE_DIR
)

dpkg-deb -x ${PACKAGE_DIR}.deb _p
tree _p
echo Package size
du -hs $PACKAGE_DIR.deb
echo Installation size
du -hs _p

# Clean-up
rm -rf _p

exit 0

