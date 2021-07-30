import QtQuick 2.0

Rectangle {
    id: rectangle1
    width: root.width
    height: root.height
    color: "#fffffe"

    Rectangle {
        width: parent.width
        height: 60
        color: "#fd9696"
        visible: unauth.text.length
        Text {
            id: unauth
            anchors.centerIn: parent
            color: "#fffffe"
            font {
                pixelSize: 36;
                family: "multi-language"
            }
        }
    }

    Image {
        id: shape
        x: 70
        y: 420
        anchors.horizontalCenter: parent.horizontalCenter
        source: "image/shape.png"
    }

    Text {
        id: qrtext
        x: 86
        y: 169
        width: 549
        height: 106
        color: "#13314f"
        text: qsTr("打开A+企业助理，扫描下方二维码\n开启配置智能门禁")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 36
    }

    Rectangle {
        id: rectangle
        x: 181
        y: 739
        width: 360
        height: 360
        color: "#00fffffe"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Image {
        id: qrImg
        anchors.horizontalCenter: rectangle.horizontalCenter
        anchors.verticalCenter: rectangle.verticalCenter
    }

    Connections {
        target: programs;
        onQrcodeChanged: {
            unauth.text = qsTr(rcode);
            qrImg.source = "file:///mnt/usr/qrcode.jpg";
        }
    }
}
