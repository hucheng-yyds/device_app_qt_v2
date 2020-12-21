import QtQuick 2.0

Item {
    width: root.width;
    height: root.height;
    property int trackId: 0
    property alias focusingVisible: focusing.visible
    property alias focusingHeight: focusing.height
    property alias focusingWidth: focusing.width
    property alias focusingY: focusing.y
    property alias focusingX: focusing.x
    Image {
        id: focusing;
        visible: false;
        source: "image/focusing.png";
    }
}
