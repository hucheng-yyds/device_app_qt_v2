import QtQuick 2.0

Item {
    id: item1
    width: root.width
    height: root.height
    property var focusingList: [focusing];
    property bool isEg;
    property bool isResultz:false;
    property bool isResulty:false;
    property int sleepTime : 5000;
    property bool fouscing : false;

    Focusing {
        id: focusing;
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
    Rectangle {
        id: tbsBg
        x: 0
        y: 0
        visible: tbs.text.length
        color: "#000000"
        opacity: 0.5
        width: parent.width
        height: parent.height
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
        id: netStatus
        x: 670
        y: 11
        source: "image/net_nosig.png";
    }

    Image {
        id: network
        x: 620
        y: 11
        source: "";
    }

//    Rectangle {
//        id: dateBg123
//        x: 270
//        y: 280
//        visible: true
//        color: "#FF72FF"
//        opacity: 0.75
//        width: 190
//        height: 380
//    }

    Rectangle {
        id: dateBg
        x: 0
        y: 1033
        visible: true
        color: "#2D72FF"
//        opacity: 0.75
        width: parent.width
        height: 204
    }

    // 主界面界面显示时间
    Text {
        id: date
        x: 215
        y: 1172
        height: 35
        width: 283
        color: "#fffffe"
        font {
            pixelSize: 38;
            family: "multi-language"
        }
        text: qsTr("")
        style: Text.Raised
    }

    Text {
        id: times
        x: 214
        y: 1025
        width: 292
        height: 81
        color: "#fffffe"
        font {
            pixelSize: 112;
            family: "multi-language"
        }
        text: qsTr("")
        style: Text.Raised
    }

    // 显示当前入库人数
    Image {
        id: image_people
        x: 37
        y: 96
        width: 30
        height: 30
        source: "image/number.png"
    }

    Text {
        id: people;
        x: 75
        y: 90
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("")
        style: Text.Raised
    }

    // 显示设备名称
    Text {
        id: corporateName
        x: 35
        y: 11
        width: 289
        color: "#fffffe"
        font {
            pixelSize: 48
            family: "multi-language"
        }
        text: qsTr("")
        style: Text.Raised
    }

    // 显示设备sn码
    Text {
        id: sn;
        x: 11
        y: 1246
        font {
            pixelSize: 20
            family: "multi-language"
        }
        color: "#666666"
        text: qsTr("")
        style: Text.Raised
    }

    // 显示ip
    Text {
        id: ip;
        x: 350
        y: 1246
        font {
            pixelSize: 20
            family: "multi-language"
        }
        color: "#666666"
        text: qsTr("")
        style: Text.Raised
    }

    // 显示版本号

    Text {
        id: version;
        x: 620
        y: 1246
        width: 79
        font {
            pixelSize: 20
            family: "multi-language"
        }
        color: "#666666"
        text: qsTr("V0.0.0")
        style: Text.Raised
    }

    // 定时器自动隐藏ui弹窗
    Timer {
        id: hideName;
        interval: 3000; running: false;
        onTriggered: {
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
        id: resultTextBg
        y: 828
        visible: resultText.text.length
        color: "#202020"
        radius: 90
        opacity: 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        width: 660
        height: 183
    }

    Text {
        id: resultText
        font {
            pixelSize: 78
            family: "multi-language"
        }
        color: "#FFFFFE"
        anchors.horizontalCenter: resultTextBg.horizontalCenter
        anchors.verticalCenter: resultTextBg.verticalCenter
        style: Text.Raised
    }

    Image {
        id: faceShow
        x: 0
        y: 0
        visible: false
        width: 720
        height: 1012
        source: "image/face.png"
    }

    Image {
        cache: false;
        id: iconFace
        x: 200
        y: 644
        visible: true
        width: 500
        height: 500
        source: ""
    }

    Connections {
        target: programs;

        onSyncSuccess: {
            sync.visible = false;
            isEg = eg;
            sleep.restart();
            console.log("onSyncSuccess");
        }
        onFaceTb: {
//            for (var i = 0; i < focusingList.length; i ++) {
//                focusingList[i].focusingVisible = false;
//                focusingList[i].trackId = 0;
//            }
            faceShow.visible = false;
            tbs.text = qsTr(text);
            standby.visible = false;
            face.visible = true;
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
            resultText.text = qsTr(showInfo)
            hideName.restart()
            sleep.restart()
        }
        onShowFaceFocuse:{
//            var flag = false;
//            for (var i = 0; i < focusingList.length; i ++) {
//                if (focusingList[i].trackId === trackId && !isTemp) {
//                    focusingList[i].focusingX = left - 20;
//                    focusingList[i].focusingY = top - 30;
//                    focusingList[i].focusingWidth = right - left + 50;
//                    focusingList[i].focusingHeight = bottom - top + 30;
//                    focusingList[i].focusingVisible = true;
//                    flag = true;
//                    break;
//                }
//                else
//                {
//                    focusingList[i].focusingVisible = false;
//                    focusingList[i].trackId = 0;
//                }
//            }
//            if (!flag && !isTemp) {
//                focusingList[index].focusingX = left - 20;
//                focusingList[index].focusingY = top - 30;
//                focusingList[index].focusingWidth = right - left + 50;
//                focusingList[index].focusingHeight = bottom - top + 30;
//                focusingList[index].focusingVisible = true;
//                focusingList[index].trackId = trackId;
//            }
            standby.visible = false;
            face.visible = true;
            sleep.restart();
        }
        onShowFaceResult: {
            faceShow.visible = true;
            standby.visible = false;
            face.visible = true;
            sleep.restart();
            console.log("==================================================")
        }
        onHideFaceFocuse:{
            faceShow.visible = false;
//            for (var i = 0; i < focusingList.length; i ++) {
//                focusingList[i].focusingVisible = false;
//                focusingList[i].trackId = 0;
//            }
        }
        onShowDeviceInfo:{
            version.text = qsTr("V" + ver);
            corporateName.text = qsTr(name);
            people.text = qsTr(number);
            ip.text = qsTr("IP: " + devIp);
            sn.text = qsTr("SN: " + devSn);
        }
        onShowIr: {
            iconFace.source = ""
            iconFace.source = "image/face_small.png";
        }
//        onSaveImage:{
//            item1.grabToImage(function(result) {
//                result.saveToFile("something.png");
//            });
//        }
        onIcResultShow: {
            hideName.restart()
            sleep.restart()
            if(2 === result)
            {
                resultText.text = qsTr(showInfo)
            }
            else if(1 === result)
            {
                resultText.text = qsTr(showInfo)
            }
            else if(0 === result)
            {
                resultText.text = qsTr(showInfo)
            }
            hideName.restart()
            standby.visible = false;
            face.visible = true;
            sleep.restart();
        }
        onTimeSync: {
            standby.g_dateCur = dataCur;
            standby.g_digitalClock = digitalClock;
            standby.g_hours = hour;
            standby.g_minutes = min;
            date.text = qsTr(dataTime);
            times.text = digitalClock;
        }
    }
}
