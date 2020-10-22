import QtQuick 2.0

Item {
    id: item1
    width: root.width
    height: root.height
    property var focusingList: [focusing/*, focusing1, focusing2, focusing3, focusing4*/];

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

    // 检测结果显示
    Rectangle {
        id: textNameBg
        y: 853
        visible: textName.text.length;
        color: "#3f3f3f"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 683
        height: 134
    }

    Text {
        id: textName
        anchors.verticalCenter: textNameBg.verticalCenter
        anchors.horizontalCenter: textNameBg.horizontalCenter
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
        visible: textNameBg.visible
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
        id: tipsBg
        y: 987
        visible: tips.text.length
        color: "#fffffe"
        anchors.horizontalCenter: parent.horizontalCenter
        width: 683
        height: 109
    }

    Text {
        id: tips
        anchors.verticalCenter: tipsBg.verticalCenter
        anchors.horizontalCenter: tipsBg.horizontalCenter
        font {
            pixelSize: 48
            family: "multi-language"
        }
        color: "#3f3f3f"
        style: Text.Raised
    }

    Connections {
        target: guiapi;
        onFaceResultShow:{

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
    }
}
