import QtQuick 2.0

Item {
    id: item1
    width: 600
    height: 1024
    property var focusingList: [focusing];
    property bool isEg;
    property bool isTemp;
    property bool isResultz:false;
    property bool isResulty:false;
    property int sleepTime : 5000;

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

    Image {
        id: netStatus
        x: 555
        y: 11
        source: "image/eth0_nosig.png";
    }

    Image {
        id: network
        x: 510
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
        height: 960
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
        x: 1
        y: 2
        width: 600
        height: 844
        source: "image/head_temp.png"
        visible: false
    }

    Rectangle {
        id: tempBg
        x: 250
        y: 490
        visible: false
        color: "#FEFFFF"
        radius: 30
        width: 300
        height: 300
    }

    // 体温正常显示绿色框
    Image {
        id: tempNormal
        x: 325
        y: 517
        source: "image/tiwenzhc.png"
        visible: isResultz
    }

    // 体温异常显示红色框
    Image {
        id: tempUnusual
        x: 325
        y: 517
        source: "image/tiwenyc.png"
        visible: isResulty
    }

    // 体温偏低
    Image {
        id: tempLow
        x: 325
        y: 517
        source: "image/tiwenlow.png"
        visible: tempInfoLow.visible
    }

    // 测温失败
    Image {
        id: tempError
        x: 325
        y: 517
        source: "image/cwshibai.png"
        visible: tempInfoError.visible
    }

    // 显示测温失败提示信息
    Text {
        id: tempInfoError
        x: 298
        y: 688
        width: 205
        height: 70
        visible: false
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#9E9E9E"
//        wrapMode: Text.WordWrap
        text: qsTr("测温中请勿\n大幅度晃动头部")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    // 显示测温偏低提示信息
    Text {
        id: tempInfoLow
        x: 328
        y: 688
        width: 144
        height: 70
        visible: false
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#9E9E9E"
        text: qsTr("检查是否\n受风力影响")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    // 显示测温中提示信息
    Text {
        id: temping
        x: 333
        y: 686
        width: 135
        height: 34
        visible: false
        font {
            pixelSize: 36
            family: "multi-language"
        }
        color: "#3F3F3F"
        text: qsTr("测温中...")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: tempingInfo
        x: 297
        y: 737
        width: 135
        height: 30
        visible: temping.visible
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#9E9E9E"
        text: qsTr("请勿离开测温区")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    AnimatedImage {
        id: cewening
        visible: temping.visible
        x: 325
        y: 517
        source: "./gifs/cewen.gif"
//        playing: true
        playing: false
    }



    // 显示体温检测结果
    Text {
        id: tempResult
        x: 320
        y: 700
        visible: false
        font {
            pixelSize: 54
            family: "multi-language"
        }
        color: isResultz ? "#3F3F3F" : "#E5533D"
        text: qsTr("")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    // 定时器 清空体温检测结果
    Timer {
        id: pose_blur_Timer;
        interval: 2000; running: false;
        onTriggered: {
            tempBg.visible = false;
            isResultz = false;
            isResulty = false;
            tempResult.visible = false;
            tempInfoError.visible = false;
            tempInfoLow.visible = false;
            idCard.visible = false;
            cardFail.visible = false;
            cardSuccess.visible = false;
            kansxt.visible = false;
        }
    }

    // 显示读卡中提示信息
    Text {
        id: reading
        x: 333
        y: 686
        width: 135
        height: 34
        visible: false
        font {
            pixelSize: 36
            family: "multi-language"
        }
        color: "#3F3F3F"
        text: qsTr("读卡中...")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: readingInfo
        x: 297
        y: 737
        width: 135
        height: 30
        visible: reading.visible
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#9E9E9E"
        text: qsTr("请勿离开读卡区")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    AnimatedImage {
        id: readingGif
        visible: reading.visible
        x: 325
        y: 517
        source: "./gifs/reading.gif"
        //        playing: true
                playing: false
    }

    // 人证失败
    Image {
        id: cardFail
        x: 325
        y: 517
        source: "image/icFail.png"
        visible: false
    }

    // 人证成功
    Image {
        id: cardSuccess
        x: 325
        y: 517
        source: "image/icSuccess.png"
        visible: false
    }

    // 刷身份证
    Image {
        id: idCard
        x: 325
        y: 517
        source: "image/idCard.png"
        visible: false
    }

    // 提醒刷身份证信息
    Text {
        id: idCardRead
        x: 327
        y: 688
        width: 144
        height: 70
        visible: idCard.visible
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#9E9E9E"
        text: qsTr("请将身份证\n放置读卡区")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    // 看摄像头
    AnimatedImage {
        id: kansxtGif
        visible: kansxt.visible
        x: 325
        y: 517
        source: "./gifs/kansxt.gif"
        //        playing: true
                playing: false
    }

    // 看摄像头信息
    Text {
        id: kansxt
        x: 342
        y: 688
        width: 144
        height: 70
        visible: idCard.visible
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#9E9E9E"
        text: qsTr("请将人脸\n正对设备")
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    // 主界面界面显示时间
    Text {
        id: date
        x: 30
        y: 16
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
        x: 29
        y: 58
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
        x: 0
        y: 201
        width: 68
        height: 41
        source: "image/public_people.png"
    }

    Text {
        id: people;
        x: 70
        y: 200
        font {
            pixelSize: 30
            family: "multi-language"
        }
        color: "#fffffe"
        text: qsTr("")
        style: Text.Raised
    }


    Image {
        id: pngShow
        visible: true
        x: 0
        y: 859
        source: "image/show.png"
    }
    AnimatedImage {
        id: gifShow
        visible: pngShow.visible
        x: 0
        y: 859
        source: "./gifs/show.gif"
                playing: true
//                playing: false
    }
    Image {
        id: pngRun
        visible: false
        x: 0
        y: 859
        source: "image/run.png"
    }
    AnimatedImage {
        id: gifRun
        visible: pngRun.visible
        x: 0
        y: 859
        source: "./gifs/run.gif"
        playing: true
//                playing: false
    }

    // 显示设备名称
    Text {
        id: corporateName
        x: 134
        y: 898
        width: 289
        color: "#fffffe"
        font {
//            bold: "Medium"
            pixelSize: 42
            family: "multi-language"
        }
        text: qsTr("")
        style: Text.Raised
    }

    Rectangle {
        id: devIndoBg
        x: 0
        y: 986
        width: 600
        height: 38
        opacity: 0.5
        color: "#000000"
    }

    // 显示设备sn码
    Text {
        id: sn;
        x: 11
        y: 990
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
        x: 300
        y: 990
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
        x: 518
        y: 990
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

    Timer {
        id: gifChange;
        interval: 1000; running: false;
        onTriggered: {
            pngRun.visible = false
            pngShow.visible = true
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
        y: 807
        visible: resultText.text.length
        color: "#000000"
        radius: 20
        opacity: 0.5
        anchors.horizontalCenter: parent.horizontalCenter
        width: 380
        height: 77
    }

    Text {
        id: resultText
        font {
            pixelSize: 42
            family: "multi-language"
        }
        color: "#16DFEC"
        anchors.left: resultTextBg.left
        anchors.leftMargin: 25
        anchors.verticalCenter: resultTextBg.verticalCenter
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
            console.log("onSyncSuccess");
        }
        onFaceTb: {
            for (var i = 0; i < focusingList.length; i ++) {
                focusingList[i].focusingVisible = false;
                focusingList[i].trackId = 0;
            }
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
            kansxt.visible = false;
            resultText.text = qsTr(showInfo)
            pngRun.visible = true
            pngShow.visible = false
            hideName.restart()
            gifChange.restart()
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
        onHideFaceFocuse:{
            for (var i = 0; i < focusingList.length; i ++) {
                focusingList[i].focusingVisible = false;
                focusingList[i].trackId = 0;
            }
        }
        onShowDeviceInfo:{
            isTemp = tempCtl;
            image_head.visible = isTemp
            version.text = qsTr("V" + ver);
            corporateName.text = qsTr(name);
            people.text = qsTr(number);
            ip.text = qsTr("IP: " + devIp);
            sn.text = qsTr("SN: " + devSn);
        }
        onIcResultShow: {
            pngRun.visible = true
            pngShow.visible = false
            hideName.restart()
            gifChange.restart()
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

        onIdCardResultShow: {
            pose_blur_Timer.restart();
            tempBg.visible = true;
            reading.visible = false;
            kansxt.visible = false;
            if(2 === flag)
            {
                cardFail.visible = false;
                resultText.text = showInfo;
                cardSuccess.visible = true;
                tempResult.text = qsTr(result)
                tempResult.visible = true;
            }
            else if(1 === flag)
            {
                resultText.text = showInfo;
                cardSuccess.visible = false;
                cardFail.visible = true;
                tempResult.text = qsTr(result)
                tempResult.visible = true;
            }
            else if(0 === flag)
            {
                resultText.text = showInfo;
                cardSuccess.visible = false;
                cardFail.visible = false;
                tempResult.visible = false;
                idCard.visible = true;
            }
            hideName.restart()
            standby.visible = false;
            face.visible = true;
            sleep.restart();
        }
        onReadIcStatus: {
            pose_blur_Timer.restart();
            idCard.visible = false;
            resultText.text = "";
            cardSuccess.visible = false;
            tempResult.visible = false;
            tempBg.visible = true;
            if(1 === flag)
            {
                kansxt.visible = false;
                tempResult.visible = false;
                cardFail.visible = false;
                reading.visible = true;
            }
            else if(2 === flag)
            {
                reading.visible = false;
                kansxt.visible = true;
            }
            else if(0 === flag)
            {
                kansxt.visible = false;
                reading.visible = false;
                cardFail.visible = true;
                tempResult.text = qsTr("请重新刷卡");
                tempResult.visible = true;
            }
            standby.visible = false;
            face.visible = true;
            sleep.restart();
        }
        onShowStartTemp: {
            idCard.visible = false;
            cardFail.visible = false;
            cardSuccess.visible = false;
            pose_blur_Timer.restart();
            isResultz = false;
            isResulty = false;
            tempInfoLow.visible = false;
            tempInfoError.visible = false;
            tempResult.visible = false;
            tempBg.visible = true;
            temping.visible = true;
        }
        onTempShow: {
            pngRun.visible = true
            pngShow.visible = false
            gifChange.restart()
            temping.visible = false
            pose_blur_Timer.restart();
            if (result === 0) {
                isResultz = false;
                isResulty = true;
                tempInfoLow.visible = false;
                tempInfoError.visible = false;
                tempResult.text = tempVal;
                tempResult.visible = true;
            } else if (result === 1) {
                isResultz = true;
                isResulty = false;
                tempInfoLow.visible = false;
                tempInfoError.visible = false;
                tempResult.text = tempVal;
                tempResult.visible = true;
            } else if(result === -1)
            {
                isResultz = false;
                isResulty = false;
                tempInfoLow.visible = true;
                tempInfoError.visible = false;
                tempResult.visible = false;
            }
            else {
                isResultz = false;
                isResulty = false;
                tempInfoLow.visible = false;
                tempInfoError.visible = true;
                tempResult.visible = false;
            }
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
