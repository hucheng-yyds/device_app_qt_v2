QT += qml quick network sql websockets widgets
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += CONFIG_CTRL_IFACE
DEFINES += CONFIG_CTRL_IFACE_UNIX
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET  = F01
MOC_DIR = $$PWD/Compile/moc
RCC_DIR = $$PWD/Compile/moc
OBJECTS_DIR = $$PWD/Compile/obj

SOURCES += main.cpp

CONFIG          += warn_off

INCLUDEPATH     += $$PWD
INCLUDEPATH     += $$PWD/external/include
INCLUDEPATH     += $$PWD/external/include/tts
INCLUDEPATH     += $$PWD/external/include/utils

LIBS += -L$$PWD/external/lib
LIBS += -L$$PWD/external/lib/qz
LIBS += -L$$PWD/external/lib/opencv

#LIBS += $$PWD/external/lib/libvio.a
#LIBS += $$PWD/external/lib/libqrcodegen.a
#LIBS += $$PWD/external/lib/libHTmcu.a
#LIBS += $$PWD/external/lib/libcurl.a

#LIBS += -lmsc -lpaho-embed-mqtt3c -lcrypto -lssl
#LIBS += -l100UD -lusb-1.0 -ltermbhf
LIBS += -lcrypto -lssl
LIBS += -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
LIBS += -lyxface -lcnn
LIBS += -Wl,-static \
    -lvio

LIBS += -Wl,-Bdynamic \
    -lmedia_utils \
    -lcedarx_aencoder \
    -ladecoder \
    -lvencoder \
    -lvenc_codec \
    -lvenc_base \
    -lMemAdapter \
    -lVE \
    -lcdc_base \
    -lISP \
    -lisp_ini \
    -lcedarxstream \
    -lion \
    -lcedarxrender \
    -lcedarx_tencoder \
    -lvdecoder \
    -lvideoengine \
    -lawh264 \
    -lawh265 \
    -lawmjpegplus \
    -l_ise_mo \
    -lmpp_vi \
    -lmpp_isp \
    -lmpp_vo \
    -lmpp_ise \
    -lmpp_component \
    -lmedia_mpp \
    -ldl \
    -lrt \
    -lpthread \
    -llog \
    -lion \
    -lsample_confparser \
    -lhwdisplay \
    -lasound \
    -lcutils \
    -lcdx_common \
    -lcdx_base \
    -lcdx_parser -lglog -lz -lcdx_stream

INCLUDEPATH     += $$PWD/hardware
INCLUDEPATH     += $$PWD/common
INCLUDEPATH     += $$PWD/functionSwitch
INCLUDEPATH     += $$PWD/mqttModule
INCLUDEPATH     += $$PWD/faceAlgorithm
INCLUDEPATH     += $$PWD/sqldatabase
INCLUDEPATH     += $$PWD/netManager
INCLUDEPATH     += $$PWD/communicateModule
INCLUDEPATH     += $$PWD/communicateExt
INCLUDEPATH     += $$PWD/temperature
INCLUDEPATH     += $$PWD/logs
INCLUDEPATH     += $$PWD/idModule
INCLUDEPATH     += $$PWD/icModule
INCLUDEPATH     += $$PWD/wgModule
INCLUDEPATH     += $$PWD/rcodeModule
INCLUDEPATH     += $$PWD/wpa

include         ($$PWD/hardware/hardware.pri)
include         ($$PWD/common/common.pri)
include         ($$PWD/functionSwitch/functionswitch.pri)
include         ($$PWD/mqttModule/mqttModule.pri)
include         ($$PWD/faceAlgorithm/facealgorithm.pri)
include         ($$PWD/sqldatabase/sqldatabase.pri)
include         ($$PWD/netManager/netmanager.pri)
include         ($$PWD/communicateModule/communicatemodule.pri)
include         ($$PWD/communicateExt/communicateExt.pri)
include         ($$PWD/temperature/temperature.pri)
include         ($$PWD/logs/log.pri)
#include         ($$PWD/idModule/idModule.pri)
include         ($$PWD/icModule/icModule.pri)
include         ($$PWD/wgModule/wgModule.pri)
include         ($$PWD/rcodeModule/rcodeModule.pri)
include         ($$PWD/wpa/wpa.pri)


#RESOURCES += main.qrc

DISTFILES += \
    qml/main.qml \
    qml/Face.qml \
    qml/Focusing.qml \
    qml/QRcode.qml \
    qml/Standby.qml

DISTFILES += \
    oldqml/main.qml \
    oldqml/Face.qml \
    oldqml/Focusing.qml \
    oldqml/QRcode.qml \
    oldqml/Standby.qml

DISTFILES += \
    5.5qml/main.qml \
    5.5qml/Face.qml \
    5.5qml/Focusing.qml \
    5.5qml/QRcode.qml \
    5.5qml/Standby.qml

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
