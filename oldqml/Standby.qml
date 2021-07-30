import QtQuick 2.4
import QtQuick.Controls 2.1

Rectangle {
    id: rectangle
    width: root.width
    height: root.height
    color: "#0e0e0e"
    property int g_hours;
    property int g_minutes;
    property alias g_digitalClock : digitalClock.text;
    property alias g_dateCur: dateCur.text;
    Item {
        id : clock
        anchors.horizontalCenter: parent.horizontalCenter;
        Item {
            anchors.horizontalCenter: parent.horizontalCenter;
            y: 109;
            width: background.width; height: background.height

            Image {
                id: background;
                source: "image/clock.png";
                visible: true;
                opacity: 0.9;
            }

            Image {
                id: hourImage;
                x: background.width/2 - 1.5; y: background.height/2 - height;
                opacity: 0.9
                source: "image/Clockwise.png"
                mipmap: true;
                transform: Rotation {
                    id: hourRotation
                    origin.x: 1; origin.y: hourImage.height;
                    angle: (g_hours * 30) + (g_minutes * 0.5)
                    Behavior on angle {
                        SpringAnimation { spring: 2; damping: 0.2; modulus: 360 }
                    }
                }
            }

            Image {
                id: minuteImage;
                x: background.width/2 - 0.5; y: background.height/2 - height;
                opacity: 0.9
                source: "image/Minutehand.png"
                mipmap: true;
                transform: Rotation {
                    id: minuteRotation
                    origin.x: 1; origin.y: minuteImage.height;
                    angle: g_minutes * 6
                    Behavior on angle {
                        SpringAnimation { spring: 2; damping: 0.2; modulus: 360 }
                    }
                }
            }
        }
    }
    Text {
        id: digitalClock
        anchors.horizontalCenter: parent.horizontalCenter
        y: 706;
        font {
            pixelSize: 150;
            family: "multi-language"
        }
        opacity: 0.9
        color: "#ffffff"
        text: qsTr(Qt.formatTime(parent.date, "hh:mm"))
    }

    Text {
        id :dateCur
        anchors.horizontalCenter: parent.horizontalCenter
        y: 898;
        font {
            pixelSize: 42
            family: "PingFang SC Light"
        }
        opacity: 0.5
        color: "#ffffff"
        text: qsTr("")
    }
}
