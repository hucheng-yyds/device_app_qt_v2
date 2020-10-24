import QtQuick 2.0

Item {
    id: item1
    width: root.width
    height: root.height
    property var focusingList: [focusing];
    Focusing {
        id: focusing;
    }
    // 显示gif动画
    Rectangle {
        visible: false
        x: 300
        y: 500
        width: 422
        height: 235
        //color: "blue"

        AnimatedImage {
            id: animated
            source: "./gifs/aa.gif"
            playing: true
        }
    }
    Rectangle {
        width: parent.width
        height: 60
        color: "black";
        opacity: 0.5;
    }

    // 主界面界面显示时间
    Text {
        id: date
        x: 290
        y: 9
        width: 219
        color: "#fffffe"
        font {
            pixelSize: 30;
            family: "multi-language"
        }
        text: qsTr(Qt.formatDateTime(standby.date, "MM-dd hh:mm"))
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        style: Text.Raised
    }

    // 显示版本号
    Image {
        id: image_version
        x: 20
        y: 15
        source: "image/public_version.png"
    }

    Text {
        id: version;
        x: 58
        y: 9
        font {
            pixelSize: 30;
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("V0.0.0")
        style: Text.Raised
    }

    Rectangle {
        id: image_rect
        x: 0
        y: 1120
        width: 800
        height: 160
        opacity: 0.5
        color: "black"
    }

    // 显示设备二维码
    Image {
        id: smallQrCode
        x: 644
        y: 1130
        sourceSize.width: 132
        sourceSize.height: 132
        Component.onCompleted: {
            source = "file:///mnt/usr/qrcode.jpg";
        }
    }

    // 显示设备名称
    Text {
        id: corporateName
        x: 19
        y: 1125
        color: "#fffffe"
        font {
            pixelSize: 36
            family: "multi-language"
        }
        text: qsTr("")
        style: Text.Raised
    }

    // 显示当前入库人数
    Image {
        id: image_people
        x: 20
        y: 1228
        source: "image/public_people.png"
    }

    Text {
        id: people;
        x: 60
        anchors.verticalCenter: image_people.verticalCenter
        font {
            pixelSize: 24
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("")
        style: Text.Raised
    }

    // 显示设备ip
    Image {
        id: image_ip
        x: 160
        y: 1228
        source: "image/public_ip.png"
    }

    Text {
        id: ip;
        x: 200
        anchors.verticalCenter: image_ip.verticalCenter
        font {
            pixelSize: 24;
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("")
        style: Text.Raised
    }

    // 显示设备sn码
    Image {
        id: image_sn
        x: 20
        y: 1183
        source: "image/public_sn.png"
    }

    Text {
        id: sn;
        x: 60
        anchors.verticalCenter: image_sn.verticalCenter
        font {
            pixelSize: 24;
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("")
        style: Text.Raised
    }

    // 定时器自动隐藏ui弹窗
    Timer {
        id: hideName;
        interval: 3200; running: false;
        onTriggered: {
            nameText.text = "";
            resultText.text = "";
        }
    }

    // 检测结果显示
    Rectangle {
        id: resultTextBg
        y: 853
        visible: resultText.text.length;
        color: "#3f3f3f"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 683
        height: 134
    }

    Text {
        id: resultText
        anchors.verticalCenter: resultTextBg.verticalCenter
        anchors.horizontalCenter: resultTextBg.horizontalCenter
        text: qsTr("")
        font {
            pixelSize: 60
            family: "multi-language"
        }
        color: "#fffffe"
        style: Text.Raised
    }

    // 姓名和人脸框显示
    Image {
        id: iconFace
        x: 90
        y: 875
        visible: nameTextBg.visible
        source: "image/icon_face.png"
    }

    Image {
        id: iconFaceBorder
        x: 90
        y: 875
        visible: iconFace.visible
        source: "image/icon_faceBorder.png"
    }

    Rectangle {
        id: nameTextBg
        y: 987
        visible: nameText.text.length
        color: "#fffffe"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 683
        height: 109
    }

    Text {
        id: nameText
        anchors.verticalCenter: nameTextBg.verticalCenter
        anchors.horizontalCenter: nameTextBg.horizontalCenter
        font {
            pixelSize: 48
            family: "multi-language"
        }
        color: "#3f3f3f"
        style: Text.Raised
    }

    Connections {
        target: programs;
        onFaceResultShow:{
            nameText.text = qsTr(name)
            resultText.text = qsTr(result)
            hideName.restart()
        }
        onShowFaceFocuse:{
            var flag = false;
            for (var i = 0; i < focusingList.length; i ++) {
                if (focusingList[i].trackId === trackId) {
                    focusingList[i].focusingX = left - 20;
                    focusingList[i].focusingY = top - 30;
                    focusingList[i].focusingWidth = right - left + 50;
                    focusingList[i].focusingHeight = bottom - top + 30;
                    focusingList[i].focusingVisible = true;
                    flag = true;
                    break;
                }
                else
                {
                    focusingList[i].focusingVisible = false;
                    focusingList[i].trackId = 0;
                }
            }
            if (!flag) {
                focusingList[index].focusingX = left - 20;
                focusingList[index].focusingY = top - 30;
                focusingList[index].focusingWidth = right - left + 50;
                focusingList[index].focusingHeight = bottom - top + 30;
                focusingList[index].focusingVisible = true;
                focusingList[index].trackId = trackId;
            }
        }
        onHideFaceFocuse:{
            for (var i = 0; i < focusingList.length; i ++) {
                focusingList[i].focusingVisible = false;
                focusingList[i].trackId = 0;
            }
        }
        onShowDeviceInfo:{
            version.text = qsTr("V" + ver);
            corporateName.text = qsTr(name);
            people.text = qsTr(number);
            ip.text = qsTr(devIp);
            sn.text = qsTr(devSn);
        }
    }
}
