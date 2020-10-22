import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0

ApplicationWindow {
    id: root;
    color: "transparent";
    visible: true
    width: 800
    height: 1280
    title: qsTr("Hello World")

    Face {
        id:face
    }

//    Standby {
//        id: standby
//        visible: false;
//    }

//    Connect {
//        id: connect
//        visible: false;
//    }

//    QRCode {
//        id:qrcode;
//        visible: false;
//    }
}

