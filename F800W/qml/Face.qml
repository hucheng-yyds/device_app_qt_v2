import QtQuick 2.0

Item {
    id: item1
    width: root.width
    height: root.height
    property var focusingList: [focusing];
    property bool isEg;
    property bool isTemp;
    property int sleepTime : 5000;

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

    // 初始化ui显示
    Text {
        id: sync
        width: 270
        height: 115
        text: qsTr("正在初始化")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font {
            pixelSize: 50
            family: "multi-language"
        }
        color: "#fffffe"
        style: Text.Raised
    }

    Image {
        id: netStatus
        x: 739
        y: 11
        source: "image/eth0_nosig.png";
    }

    Image {
        id: network
        x: 690
        y: 11
        source: "";
    }

    Rectangle {
        id: tbsBg
        x: 0
        y: 60
        visible: tbs.text.length
        color: "#000000"
        opacity: 0.5
        width: parent.width
        height: 1060
    }

    Text {
        id: tbs
        anchors.verticalCenter: tbsBg.verticalCenter
        anchors.horizontalCenter: tbsBg.horizontalCenter
        font {
            pixelSize: 48
            family: "multi-language"
        }
        color: "#fffffe"
        style: Text.Raised
    }

    Image {
        id: image_head
        y: 60;
        source: "image/temp_head.png"
        visible: false
    }

    // 体温正常显示绿色框
    Image {
        id: tempNormal
        y: 60
        source: "image/temp_normal.png"
        visible: false
    }

    // 体温异常显示红色框
    Image {
        id: tempUnusual
        y: 60
        source: "image/temp_unusual.png"
        visible: false
    }

    // 显示体温检测结果
    Text {
        id: pose_blur
        y: 89
        font {
            pixelSize: 48
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    // 定时器 清空体温检测结果
    Timer {
        id: pose_blur_Timer;
        interval: 2200; running: false;
        onTriggered: {
            pose_blur.text = qsTr("");
            tempNormal.visible = false;
            tempUnusual.visible = false;
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
        text: qsTr()
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

    // 定时器待机界面启动
    Timer {
        id: sleep;
        interval: sleepTime; running: false;
        onTriggered: {
            standby.visible = true;
            face.visible = false;
        }
    }

    // 检测结果显示
    Rectangle {
        id: nameTextBg
        y: 853
        visible: resultText.text.length;
        color: "#3f3f3f"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 683
        height: 134
    }

    Text {
        id: nameText
        anchors.verticalCenter: nameTextBg.verticalCenter
        anchors.horizontalCenter: nameTextBg.horizontalCenter
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
        id: resultTextBg
        y: 987
        visible: resultText.text.length
        color: "#fffffe"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 683
        height: 109
    }

    Text {
        id: resultText
        anchors.verticalCenter: resultTextBg.verticalCenter
        anchors.horizontalCenter: resultTextBg.horizontalCenter
        font {
            pixelSize: 48
            family: "multi-language"
        }
        color: "#3f3f3f"
        style: Text.Raised
    }

    Connections {
        target: programs;

        onSyncSuccess: {
            sync.visible = false;
            isEg = eg;
            isTemp = temp;
            if (eg) {
                iconFace.source = "image/icon_face.png"
            } else {
                iconFace.source = "image/icon_temp.png"
            }
            image_head.visible = isTemp
            sleep.restart();
        }
        onFaceTb: {
            tbs.text = qsTr(text);
        }

        onNetworkChanged: {
            if (net) {
                network.source = "image/online.png";
            } else {
                network.source = "";
            }
            switch (type) {
                case 0:
                    netStatus.source = "image/wifi1.png";
                    break;
                case 1:
                    netStatus.source = "image/wifi2.png";
                    break;
                case 2:
                    netStatus.source = "image/wifi3.png";
                    break;
                case 3:
                    netStatus.source = "image/4g1.png";
                    break;
                case 4:
                    netStatus.source = "image/network.png";
                    break;
                case 5:
                    netStatus.source = "image/net_nosig.png";
                    break;
            }
        }

        onFaceResultShow:{
            nameText.text = qsTr(name)
            resultText.text = qsTr(result)
            hideName.restart()
            sleep.restart()
        }
        onShowFaceFocuse:{
            var flag = false;
            for (var i = 0; i < focusingList.length; i ++) {
                if (focusingList[i].trackId === trackId && !isTemp) {
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
            if (!flag && !isTemp) {
                focusingList[index].focusingX = left - 20;
                focusingList[index].focusingY = top - 30;
                focusingList[index].focusingWidth = right - left + 50;
                focusingList[index].focusingHeight = bottom - top + 30;
                focusingList[index].focusingVisible = true;
                focusingList[index].trackId = trackId;
            }
            standby.visible = false;
            face.visible = true;
            sleep.restart();
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
        onShowStartTemp: {
            resultText.text = qsTr("正在测温")
        }
        onTempShow: {
            pose_blur_Timer.restart();
            if (result === 0) {
                resultText.text = tempVal;
                tempNormal.visible = false;
                tempUnusual.visible = true;
                pose_blur.text = qsTr("体温异常");
            } else if (result === 1) {
                resultText.text = tempVal;
                tempNormal.visible = true;
                tempUnusual.visible = false;
                pose_blur.text = qsTr("体温正常");
            } else if(result === -1)
            {
                resultText.text = qsTr("体温偏低");
            }
            else {
                resultText.text = qsTr("请重新测温");
                pose_blur.text = qsTr("请露出额头或摘下眼镜");
            }
            sleep.restart();
        }
        onTimeSync: {
            standby.g_dateCur = dataCur;
            standby.g_digitalClock = digitalClock;
            standby.g_hours = hour;
            standby.g_minutes = min;
            date.text = qsTr(dataTime);
        }
    }
}
