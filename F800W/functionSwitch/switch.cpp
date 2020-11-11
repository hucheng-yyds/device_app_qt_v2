#include "switch.h"

SwitchCtl* SwitchCtl::m_Instance = nullptr;

SwitchCtl::SwitchCtl()
{
    m_faceThreshold = 72.0;
    m_tempFlag = false;
    m_sync = false;
    m_upgrade = false;
    m_netStatus = false;
    bool status = QFile::exists("./switch.json");
    qt_debug() << "====================================================" << status;
    if(!status)
    {
        setSwitchDefault();
    }
    else {
        QJsonObject userObj, netObj, identifyObj, switchObj, serverObj, wifiObj;
        QJsonObject obj = readSwitchParam();
        qt_debug() << obj;
        userObj = obj.value("user").toObject();
        netObj = obj.value("net").toObject();
        identifyObj = obj.value("identify").toObject();
        switchObj = obj.value("switch").toObject();
        serverObj = obj.value("server").toObject();
        wifiObj = obj.value("wifi").toObject();

        m_faceDoorCtl = switchObj.value("faceDoorCtl").toBool();
        m_tempCtl = switchObj.value("tempCtl").toBool();
        m_loose = switchObj.value("loose").toBool();
        m_ir = switchObj.value("ir").toBool();
        m_vi = switchObj.value("vi").toBool();

        m_identifyWaitTime = identifyObj.value("identifyWaitTime").toInt();
        m_idcardValue = identifyObj.value("idcardValue").toDouble();

        m_ipMode = netObj.value("ipMode").toBool();
        m_manualIp = netObj.value("manualIp").toString();
        m_manualGateway = netObj.value("manualGateway").toString();
        m_manualNetmask = netObj.value("manualNetmask").toString();
        m_manualDns = netObj.value("manualDns").toString();

        m_protocol = serverObj.value("protocol").toBool();
        m_tcpAddr = serverObj.value("tcpAddr").toString();
        m_tcpPort = serverObj.value("tcpPort").toInt();
        m_httpAddr = serverObj.value("httpAddr").toString();
        m_tcpTimeout = serverObj.value("tcpTimeout").toInt();
        m_ntpAddr = serverObj.value("ntpAddr").toString();
        m_passwd = serverObj.value("passwd").toString();

        m_tempComp = userObj.value("tempComp").toDouble();
        m_warnValue = userObj.value("warnValue").toDouble();
        m_timeZone = userObj.value("timeZone").toString();
        m_openMode = userObj.value("openMode").toString();
        m_identifyDistance = userObj.value("identifyDistance").toInt();
        m_doorDelayTime = userObj.value("doorDelayTime").toInt();
        m_helet = userObj.value("helet").toBool();
        m_mask = userObj.value("mask").toInt();
        m_showIc = userObj.value("showIc").toBool();
        m_fahrenheit = userObj.value("fahrenheit").toBool();
        m_irLightCtl = userObj.value("irLightCtl").toString();
        m_bgrLightCtl = userObj.value("bgrLightCtl").toString();
        m_uploadImageCtl = userObj.value("uploadImageCtl").toBool();
        m_uploadStrangerCtl = userObj.value("uploadStrangerCtl").toBool();
        m_language = userObj.value("language").toInt();
        m_devName = userObj.value("devName").toString();
        m_nameMask = userObj.value("nameMask").toInt();
        m_tts = userObj.value("tts").toBool();
        m_tempValueBroadcast = userObj.value("tempValueBroadcast").toBool();
        m_rcode = userObj.value("rcode").toBool();
        m_volume = userObj.value("volume").toInt();
        m_wifiName = wifiObj.value("wifiName").toString();
        m_wifiPwd = wifiObj.value("wifiPwd").toString();
    }
    status = QFile::exists("./screen.json");
    qt_debug() << "---------------------------------------------------------" << status;
    if(!status)
    {
        setScreenDefault();
    }
    else {
        QJsonObject obj = readScreenParam();
        m_angle = obj.value("angle").toInt();
        m_camera = obj.value("camera").toInt();
        m_screen = obj.value("screen").toInt();
        m_sn = obj.value("sn").toString();
        qt_debug() << m_angle << m_camera << m_screen << m_sn;
    }
}

void SwitchCtl::saveSwitchParam()
{
    QFile file("./switch.json");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonObject obj, userObj, netObj, identifyObj, switchObj, serverObj, wifiObj;
    switchObj.insert("faceDoorCtl", m_faceDoorCtl);
    switchObj.insert("tempCtl", m_tempCtl);
    switchObj.insert("ir", m_ir);
    switchObj.insert("loose", m_loose);
    switchObj.insert("vi", m_vi);

    identifyObj.insert("idcardValue", m_idcardValue);
    identifyObj.insert("identifyWaitTime", m_identifyWaitTime);

    serverObj.insert("protocol", m_protocol);
    serverObj.insert("tcpAddr", m_tcpAddr);
    serverObj.insert("tcpPort", m_tcpPort);
    serverObj.insert("tcpTimeout", m_tcpTimeout);
    serverObj.insert("httpAddr", m_httpAddr);
    serverObj.insert("ntpAddr", m_ntpAddr);
    serverObj.insert("passwd", m_passwd);

    netObj.insert("ipMode", m_ipMode);
    netObj.insert("manualIp", m_manualIp);
    netObj.insert("manualGateway", m_manualGateway);
    netObj.insert("manualNetmask", m_manualNetmask);
    netObj.insert("manualDns", m_manualDns);

    userObj.insert("openMode", m_openMode);
    userObj.insert("tempComp", m_tempComp);
    userObj.insert("warnValue", m_warnValue);
    userObj.insert("timeZone", m_timeZone);
    userObj.insert("identifyDistance", m_identifyDistance);
    userObj.insert("doorDelayTime", m_doorDelayTime);
    userObj.insert("helet", m_helet);
    userObj.insert("mask", m_mask);
    userObj.insert("showIc", m_showIc);
    userObj.insert("fahrenheit", m_fahrenheit);
    userObj.insert("irLightCtl", m_irLightCtl);
    userObj.insert("bgrLightCtl", m_bgrLightCtl);
    userObj.insert("uploadImageCtl", m_uploadImageCtl);
    userObj.insert("uploadStrangerCtl", m_uploadStrangerCtl);
    userObj.insert("language", m_language);
    userObj.insert("devName", m_devName);
    userObj.insert("tts", m_tts);
    userObj.insert("tempValueBroadcast", m_tempValueBroadcast);
    userObj.insert("rcode", m_rcode);
    userObj.insert("volume", m_volume);

    wifiObj.insert("wifiName", m_wifiName);
    wifiObj.insert("wifiPwd", m_wifiPwd);

    obj.insert("user", userObj);
    obj.insert("net", netObj);
    obj.insert("server", serverObj);
    obj.insert("identify", identifyObj);
    obj.insert("switch", switchObj);
    obj.insert("wifi", wifiObj);
    QJsonDocument jdoc(obj);
    file.seek(0);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}

QJsonObject SwitchCtl::readSwitchParam()
{
    QFile file("./switch.json");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }

    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    file.close();
    return obj;
}

void SwitchCtl::setSwitchDefault()
{
    QFile file("./switch.json");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonObject obj, userObj, netObj, identifyObj, switchObj, serverObj, wifiObj;
    m_faceDoorCtl = true;
    m_tempCtl = true;
    m_loose = false;
    m_ir = true;
    m_vi = false;
    m_identifyWaitTime = 3;
    m_idcardValue = 60;
    m_ipMode = true;
    m_manualIp = "192.168.100.2";
    m_manualGateway = "192.168.100.1";
    m_manualNetmask = "255.255.255.0";
    m_manualDns = "114.114.114.0";

    m_protocol = true;
    m_tcpAddr = "";
    m_tcpPort = 8777;
    m_httpAddr = "http://120.79.147.36:8086/starr-web";
    m_tcpTimeout = 3;
    m_ntpAddr = "120.25.108.11";
    m_passwd = "";

    m_tempComp = 0.0;
    m_warnValue = 37.3;
    m_timeZone = "(UTC+08:00)";
    m_openMode = "FaceTemp";
    m_identifyDistance = 1;
    m_doorDelayTime = 3;
    m_helet = false;
    m_mask = 0;
    m_showIc = false;
    m_fahrenheit = false;
    m_irLightCtl = "";
    m_bgrLightCtl = "";
    m_uploadImageCtl = true;
    m_uploadStrangerCtl = true;
    m_language = 0;
    m_devName = "F800W_F800";
    m_nameMask = 0;
    m_tts = false;
    m_tempValueBroadcast = false;
    m_rcode = false;
    m_volume = 100;
    m_wifiName = "";
    m_wifiPwd = "";

    switchObj.insert("faceDoorCtl", m_faceDoorCtl);
    switchObj.insert("tempCtl", m_tempCtl);
    switchObj.insert("ir", m_ir);
    switchObj.insert("loose", m_loose);
    switchObj.insert("vi", m_vi);

    identifyObj.insert("idcardValue", m_idcardValue);
    identifyObj.insert("identifyWaitTime", m_identifyWaitTime);

    serverObj.insert("protocol", m_protocol);
    serverObj.insert("tcpAddr", m_tcpAddr);
    serverObj.insert("tcpPort", m_tcpPort);
    serverObj.insert("tcpTimeout", m_tcpTimeout);
    serverObj.insert("httpAddr", m_httpAddr);
    serverObj.insert("ntpAddr", m_ntpAddr);
    serverObj.insert("passwd", m_passwd);

    netObj.insert("ipMode", m_ipMode);
    netObj.insert("manualIp", m_manualIp);
    netObj.insert("manualGateway", m_manualGateway);
    netObj.insert("manualNetmask", m_manualNetmask);
    netObj.insert("manualDns", m_manualDns);

    userObj.insert("openMode", m_openMode);
    userObj.insert("tempComp", m_tempComp);
    userObj.insert("warnValue", m_warnValue);
    userObj.insert("timeZone", m_timeZone);
    userObj.insert("identifyDistance", m_identifyDistance);
    userObj.insert("doorDelayTime", m_doorDelayTime);
    userObj.insert("helet", m_helet);
    userObj.insert("mask", m_mask);
    userObj.insert("showIc", m_showIc);
    userObj.insert("fahrenheit", m_fahrenheit);
    userObj.insert("irLightCtl", m_irLightCtl);
    userObj.insert("bgrLightCtl", m_bgrLightCtl);
    userObj.insert("uploadImageCtl", m_uploadImageCtl);
    userObj.insert("uploadStrangerCtl", m_uploadStrangerCtl);
    userObj.insert("language", m_language);
    userObj.insert("devName", m_devName);
    userObj.insert("tts", m_tts);
    userObj.insert("tempValueBroadcast", m_tempValueBroadcast);
    userObj.insert("rcode", m_rcode);
    userObj.insert("volume", m_volume);

    wifiObj.insert("wifiName", m_wifiName);
    wifiObj.insert("wifiPwd", m_wifiPwd);

    obj.insert("user", userObj);
    obj.insert("net", netObj);
    obj.insert("server", serverObj);
    obj.insert("identify", identifyObj);
    obj.insert("switch", switchObj);
    obj.insert("wifi", wifiObj);

    QJsonDocument jdoc(obj);
    file.seek(0);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}

void SwitchCtl::saveSreenParam()
{
    QFile file("./screen.json");
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonObject obj;
    obj.insert("sn", m_sn);
    obj.insert("angle", m_angle);
    obj.insert("camera", m_camera);
    obj.insert("screen", m_screen);
    QJsonDocument jdoc(obj);
    file.seek(0);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}

QJsonObject SwitchCtl::readScreenParam()
{
    QFile file("./screen.json");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    file.close();
    return obj;
}

void SwitchCtl::setScreenDefault()
{
    QFile file("./screen.json");
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonObject obj;
    m_angle = 1;
    m_camera = 22;
    m_screen = 4;
    obj.insert("sn", "");
    obj.insert("angle", m_angle);
    obj.insert("camera", m_camera);
    obj.insert("screen", m_screen);
    QJsonDocument jdoc(obj);
    file.seek(0);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}
