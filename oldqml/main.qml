import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2

ApplicationWindow {
    id: root;
    color: "transparent";
    visible: true
    width: 600
    height: 1024
    title: qsTr("Hello World")

    Face {
        id:face
    }

    Standby {
        id: standby
        visible: false;
    }
}

