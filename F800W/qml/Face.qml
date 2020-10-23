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
    }
}
