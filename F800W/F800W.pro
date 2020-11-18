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
INCLUDEPATH     += $$PWD/external/include/hi
INCLUDEPATH     += $$PWD/external/include/tts

LIBS += -L$$PWD/external/lib
LIBS += -L$$PWD/external/lib/hi
LIBS += -L$$PWD/external/lib/opencv

LIBS += $$PWD/external/lib/libvio.a
LIBS += $$PWD/external/lib/libqrcodegen.a
LIBS += $$PWD/external/lib/libHTmcu.a
LIBS += $$PWD/external/lib/libcurl.a

LIBS += -lisp -lmpi -l_hidehaze -l_hidrc -l_hildci -lhifisheyecalibrate -lsecurec -lhi_cipher -lsvpruntime -lnnie -l_hiae -l_hiawb
LIBS += -lsns_imx327 -lsns_imx327_2l -lsns_imx307 -lsns_imx307_2l -lsns_imx335 -lsns_imx458 -lsns_mn34220 -lsns_os05a -lsns_c2395
LIBS += -ldnvqe -lhdmi -lhi_cipher -live -lmd -ltde -lupvqe -lVoiceEngine -lgomp
LIBS += -laaccomm -laacdec -laacenc -laacsbrdec -laacsbrenc -ldl
LIBS += -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d -lopencv_features2d  -lopencv_flann -lopencv_highgui -lopencv_videoio
LIBS += -lmsc -lpaho-embed-mqtt3c -lcrypto -lssl
LIBS += -lfacesdk_dv300
LIBS += -l100UD -lusb-1.0 -lidr220s_reader

INCLUDEPATH     += $$PWD/hardware
INCLUDEPATH     += $$PWD/common
INCLUDEPATH     += $$PWD/functionSwitch
INCLUDEPATH     += $$PWD/mqtt
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
include         ($$PWD/mqtt/mqtt.pri)
include         ($$PWD/faceAlgorithm/facealgorithm.pri)
include         ($$PWD/sqldatabase/sqldatabase.pri)
include         ($$PWD/netManager/netmanager.pri)
include         ($$PWD/communicateModule/communicatemodule.pri)
include         ($$PWD/communicateExt/communicateExt.pri)
include         ($$PWD/temperature/temperature.pri)
include         ($$PWD/logs/log.pri)
include         ($$PWD/idModule/idModule.pri)
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


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
