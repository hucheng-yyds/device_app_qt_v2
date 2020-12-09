import QtQuick 2.0

Item {
    id: item1
    width: root.width
    height: root.height
    property var focusingList: [focusing];
    property int sleepTime : 5000;

    Focusing {
        id: focusing;
    }

    Rectangle {
        width: parent.width
        height: 60
        color: "black";
        opacity: 0.5;
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
            pixelSize: 46
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

    Image {
        id: image_version
        x: 14
        y: 13
        source: "image/public_version.png"
    }

    Text {
        id: version;
        x: 53
        y: 17
        font {
            pixelSize: 28;
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("V0.1.0")
        style: Text.Raised
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
        text: qsTr("")
        style: Text.Raised
    }

    Rectangle {
        id: textNameBg
        y: 892
        visible: textName.text.length;
        color: "#3f3f3f"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 622
        height: 122
    }

    Text {
        id: textName
        x: 184
        y: 930
        text: qsTr("")
        font {
            pixelSize: 58
            family: "multi-language"
        }
        color: "#fffffe"
        style: Text.Raised
    }
    Image {
        id: iconFace
        x: 102
        y: 932
        visible: textNameBg.visible
        source: "image/icon_face.png"
    }

    Image {
        id: iconFaceBorder
        x: 78
        y: 912
        visible: iconFace.visible
        source: "image/icon_faceBorder.png"
    }

    Rectangle {
        id: tipsBg
        y: 1014
        visible: tips.text.length
        color: "#fffffe"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 622
        height: 122
    }

    Text {
        id: tips
        anchors.verticalCenter: tipsBg.verticalCenter
        anchors.horizontalCenter: tipsBg.horizontalCenter
        font {
            pixelSize: 43
            family: "multi-language"
        }
        color: "#3f3f3f"
        style: Text.Raised
    }

    Rectangle {
        id: image_rect
        x: 0
        y: 1136
        width: 720
        height: 144
        opacity: 0.5
        color: "black"
    }
    Text {
        id: corporateName
        x: 14
        y: 1149
        color: "#fffffe"
        font {
            pixelSize: 32
            family: "multi-language"
        }
        text: qsTr("")
        style: Text.Raised
    }

    Image {
        id: image_people
        x: 14
        y: 1243
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

    Image {
        id: image_ip
        x: 140
        y: 1245
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

    Image {
        id: image_sn
        x: 14
        y: 1196
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
        interval: 3000; running: false;
        onTriggered: {
            tips.text = "";
            textName.text = "";
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

    Connections {
        target: programs;

        onSyncSuccess: {
            sync.visible = false;
            iconFace.source = "image/icon_face.png"
            sleep.restart();
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
                    netStatus.source = "image/4g2.png";
                    break;
                case 5:
                    netStatus.source = "image/4g3.png";
                    break;
                case 6:
                    netStatus.source = "image/network.png";
                    break;
                case 7:
                    netStatus.source = "image/net_nosig.png";
                    break;
            }
        }

        onFaceResultShow:{
            textName.text = qsTr(name);
            tips.text = qsTr(result);
            hideName.restart()
            sleep.restart()
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
            version.text = qsTr("T" + ver);
            corporateName.text = qsTr(name);
            people.text = qsTr(number);
            ip.text = qsTr(devIp);
            sn.text = qsTr(devSn);
            iconFace.source = "image/icon_face.png"
        }
        onIcResultShow: {
            if(0 === result)
            {
                textName.text = qsTr(cardNo)
                tips.text = qsTr("请联系管理员")
            }
            else if(1 === result)
            {
                textName.text = qsTr(cardNo)
                tips.text = qsTr("认证通过")
            }
            hideName.restart()
            standby.visible = false;
            face.visible = true;
            sleep.restart();
        }

        onIdCardResultShow: {
            textName.text = qsTr(name)
            tips.text = qsTr(result)         
            hideName.restart()
            standby.visible = false;
            face.visible = true;
            sleep.restart();
        }
        onReadIcStatus: {
            if(1 === flag)
            {
                textName.text = qsTr("未注册")
                tips.text = qsTr("正在读卡...")
            }
            else if(0 === flag)
            {
                textName.text = qsTr("未注册")
                tips.text = qsTr("读卡失败")
            }
            standby.visible = false;
            face.visible = true;
            sleep.restart();
        }
        onTimeSync: {
            standby.g_dateCur = dataCur;
            standby.g_digitalClock = digitalClock;
            standby.g_hours = hour;
            standby.g_minutes = min;
            date.text = digitalClock;
        }
    }
}
