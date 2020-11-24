#include "ToolTcpServer.h"
#include "CmdsKey.h"
#include <QDebug>
#include "switch.h"
#include "sqldatabase.h"
#include "datashare.h"

ToolTcpServer::ToolTcpServer()
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnect()));
    if(m_tcpServer->listen(QHostAddress::Any, 8000))
    {
        qt_debug()<<"wait connected ";
    }

    dataSize = 0;
    dataArray.clear();
    void onGetDevIp(const QString &ver, const QString &name, const QString &number, const QString &devIp, const QString &devSn);

    m_udpServer.start();
    system("rm base64SaveFile.txt");
    system("rm update.tar.xz");

    system("rm voiceBase64SaveFile.txt");
    system("rm Voiceupdate.tar.xz");
}

void ToolTcpServer::onGetTempResponse(QByteArray dat)
{
    QJsonObject jaSonObject;
    qt_debug() << dat.toHex();
    jaSonObject.insert("msgType","2");
    jaSonObject.insert("data",QString(dat.toHex()));
    ResponseDataToTool(Dev_TemCalibration_response,jaSonObject);
}

void ToolTcpServer::onCaptureCamPicture(QByteArray dat)//抓取到图片的数据
{
    QJsonObject jaSonObject;
    jaSonObject.insert("data",QString(dat));
    ResponseDataToTool(Dev_Msg_request,jaSonObject);
}

void ToolTcpServer::onCamCalibration()
{
    QJsonObject jaSonObject;
    ResponseDataToTool(Dev_CameraCalibration_response,jaSonObject);
}

void ToolTcpServer::onGetRealTimeLog(QString dat)
{
    QJsonObject jaSonObject;
    jaSonObject.insert("msgType","0");
    jaSonObject.insert("cmd","0");
    jaSonObject.insert("data",dat);
    ResponseDataToTool(Dev_Debugging_response,jaSonObject);
}

void ToolTcpServer::onGetTempHardwareInfo(QJsonObject dat )
{
   ResponseDataToTool(Dev_TemCalibration_response,dat);
}

static int bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

static QByteArray intToByte(int num)
{
    //qt_debug()<<num;
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[3] = (uchar) ((0xff000000 & num) >> 24);
    abyte0[2] = (uchar) ((0x00ff0000 & num) >> 16);
    abyte0[1] = (uchar) ((0x0000ff00 & num) >> 8);
    abyte0[0] = (uchar) (0x000000ff & num);
   // qt_debug()<<abyte0<<(0x000000ff & num)<<((0x0000ff00 & num) >> 8)<<((0x00ff0000 & num) >> 16)<<((0xff000000 & num) >> 24);
    return abyte0;
}

void ToolTcpServer::onNewConnect(void)
{
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    QString ip = m_tcpSocket->peerAddress().toString();
    qint16 port = m_tcpSocket->peerPort();
    qt_debug()<<ip<<port;
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(onTcpRead()));
    connect(m_tcpSocket,&QTcpSocket::stateChanged,this,&ToolTcpServer::OnStateChanged);
    sendSNtoClient();
}

void ToolTcpServer::OnStateChanged(QAbstractSocket::SocketState state)
{
    QString stateStr = "";
//    qt_debug()<< "xxxxxxxxxxxxxxxxxxxxxxxxxxxx" << state;
    switch(state)
    {
        case QAbstractSocket::HostLookupState:{
            stateStr = tr("请求中...");
             //m_bConnected = 1;
              //  emit sigState(Socket_State::REQUESTING);
            }
            break;

        case QAbstractSocket::ConnectingState:{
            stateStr = tr("连接中...请稍后");
            //m_bConnected = 2;
               //  emit sigState(Socket_State::CONNITING);
            }
            break;
         case QAbstractSocket::ConnectedState:
        {
            stateStr = tr("已连接");
            //m_bConnected = 3;
            emit sigToolTcpStateChange(true);
        }
            break;
        case QAbstractSocket::ClosingState:
        {
            stateStr = tr("已关闭");
            // m_bConnected = 0;
            emit sigToolTcpStateChange(false);
        }
            break;
        case QAbstractSocket::UnconnectedState:
        {
            stateStr = tr("连接断开");
            // m_bConnected = 0;
          emit sigToolTcpStateChange(false);
        }
            break;

        default:
            break;
    }
}

void ToolTcpServer::onTcpRead(void)
{
    QByteArray recData = m_tcpSocket->readAll();
    if (!(recData.left(4)=="OFLN") && !dataSize) {
        qt_debug() << "data format error !";
        return ;
    }
    int recSize = recData.size() - 11;
    if(!dataSize) {
        dataSize = bytesToInt(recData.mid(7,4));
        if (dataSize == recSize) {
            qt_debug() << recSize << dataSize;
            parseData(recData);
        } else {
            qt_debug() << dataSize;
            dataSize -= recSize;
            dataArray.append(recData);
        }
    } else {
        recSize = recData.size();
        if (dataSize < recSize) {
            QByteArray end = recData.left(dataSize);
            recData.remove(0, dataSize);
            qt_debug() << dataSize /*<< end*/ << recData.left(20);
            dataArray.append(end);
            parseData(dataArray);
            dataArray.append(recData);
        } else if (dataSize > recSize){
            qt_debug() << dataSize << recSize;
            dataSize -= recSize;
            dataArray.append(recData);
        } else {
            //qt_debug() << "===========";
            dataArray.append(recData);
            parseData(dataArray);
        }
    }
}

void ToolTcpServer::onTempInfo(QByteArray tempInfo)
{
    if(tempInfo.count() > 0)
    {
        QByteArray sendData;
        sendData.append("OFLN");
        sendData.append(uchar(1));
        sendData.append(uchar(1));
        sendData.append(uchar(2));
        sendData.append(intToByte(tempInfo.length()));
        sendData.append(tempInfo);
        m_tcpSocket->write(sendData);
        m_tcpSocket->flush();
    }
}

void ToolTcpServer::setParameters(QJsonObject & data,QString msgType,QString cmdStr)
{
    // 手动配置模式ip
    if(data.contains(key_manual_config))
    {
        bool on = data.value(key_manual_config).toBool();
        if(!on){
            switchCtl->m_ipMode = false;
            if(data.contains(key_manual_ip)&&
                    data.contains(key_manual_netmask)&&
                      data.contains(key_manual_route)&&
                    data.contains(key_manual_dns)){

                    if(data.value(key_manual_config).toString()!=""&&
                            data.value(key_manual_netmask).toString()!=""&&
                                data.value(key_manual_route).toString()!=""&&
                            data.value(key_manual_dns).toString()!=""){

                        switchCtl->m_manualIp = data.value(key_manual_config).toString();
                        switchCtl->m_manualGateway =data.value(key_manual_netmask).toString();
                        switchCtl->m_manualNetmask =data.value(key_manual_route).toString();
                        switchCtl->m_manualDns =data.value(key_manual_dns).toString();

                    }else
                          switchCtl->m_ipMode = true;
            }else   switchCtl->m_ipMode = true;
        }
        else {
             switchCtl->m_ipMode = true;
        }
    }

    // 定时息屏时间 单位秒最小3秒钟
    if(data.contains(key_closeScreenTime))
    {
        switchCtl->m_closeScreenTime = data.value(key_closeScreenTime).toInt();
        if(switchCtl->m_closeScreenTime<3)
        {
            switchCtl->m_closeScreenTime = 3;//s
        }
    }
    // 后台通信协议开关 true:tcp协议，false:http协议
    if(data.contains(key_protocol))
    {
        switchCtl->m_protocol = data.value(key_protocol).toBool();
        if(switchCtl->m_protocol )//tcp
        {
            //Tcp ip
            if(data.contains(key_server_ip))
            {
                QString newIP = data.value(key_server_ip).toString();
                if(newIP != switchCtl->m_tcpAddr)
                {
                   switchCtl->m_tcpAddr = newIP;
                   system("rm *.db");
                }
            }

            //Tcp Port
            if(data.contains(key_server_port))
            {
                int newPort = data.value(key_server_port).toInt();
                if(newPort != switchCtl->m_tcpPort)
                {
                   switchCtl->m_tcpPort = newPort;
                   system("rm *.db");
                }
            }
        }
        else{ //http
            if(data.contains(key_httpAddr))
            {
                switchCtl->m_httpAddr = data.value(key_httpAddr).toString();
            }
        }
    }

    // ntp服务器地址
    if(data.contains(key_ntpAddr))
    {
        switchCtl->m_ntpAddr = data.value(key_ntpAddr).toString();
    }
    // 测温告警值
    if(data.contains(key_temp_warnValue))
    {
        switchCtl->m_warnValue = data.value(key_temp_warnValue).toDouble();
    }

    // 温度补偿值
    if(data.contains(key_temp_comp))
    {
        switchCtl->m_tempComp = data.value(key_temp_comp).toDouble();
    }

    // 开门条件
    if(data.contains(key_openMode))
    {
        switchCtl->m_openMode = data.value(key_openMode).toString();
    }
    // 识别距离
    if(data.contains(key_identifyDistance))
    {
        switchCtl->m_identifyDistance = data.value(key_identifyDistance).toInt();
    }
    // 开门等待时间 单位秒
    if(data.contains(key_doorDelayTime))
    {
        int doorTime = data.value(key_doorDelayTime).toInt();
        switchCtl->m_doorDelayTime = (doorTime < 2 ? 2 : doorTime);
    }
    // 安全帽开关
    if(data.contains(key_helet))
    {
        switchCtl->m_helet = data.value(key_helet).toBool();
    }
    // 口罩开关 0:表示关闭 1:提醒 2:检测
    if(data.contains(key_mask))
    {
        switchCtl->m_mask = data.value(key_mask).toInt();
    }
    // 显示ic卡号 开关
    if(data.contains(key_showIc))
    {
        switchCtl->m_showIc = data.value(key_showIc).toBool();
    }
    if(data.contains(key_fahrenheit))
    {
        switchCtl->m_fahrenheit = data.value(key_fahrenheit).toBool();
    }
    if(data.contains(key_irLightCtl))
    {
        switchCtl->m_irLightCtl = data.value(key_irLightCtl).toString();
    }
    if(data.contains(key_bgrLightCtl))
    {
        switchCtl->m_bgrLightCtl = data.value(key_bgrLightCtl).toString();
    }
    if(data.contains(key_uploadImageCtl))
    {
        switchCtl->m_uploadImageCtl = data.value(key_uploadImageCtl).toBool();
    }
    if(data.contains(key_uploadStrangerCtl))
    {
        switchCtl->m_uploadStrangerCtl = data.value(key_uploadStrangerCtl).toBool();
    }
    if(data.contains(key_language))
    {
        switchCtl->m_language = data.value(key_language).toInt();
    }
    if(data.contains(key_devName))
    {
        switchCtl->m_devName = data.value(key_devName).toString();
    }
    if(data.contains(key_nameMask))
    {
        switchCtl->m_nameMask= data.value(key_nameMask).toInt();
    }
    if(data.contains(key_tts))
    {
        switchCtl->m_tts = data.value(key_tts).toBool();
    }
    if(data.contains(key_tempValueBroadcast))
    {
        switchCtl->m_tempValueBroadcast = data.value(key_tempValueBroadcast).toBool();
    }
    if(data.contains(key_rcode))
    {
        switchCtl->m_rcode = data.value(key_rcode).toInt();
    }
    if(data.contains(key_volume))
    {
        switchCtl->m_volume = data.value(key_volume).toInt();
    }
    if(data.contains(key_faceDoorCtl))
    {
        switchCtl->m_faceDoorCtl = data.value(key_faceDoorCtl).toBool();
    }
    if(data.contains(key_tempCtl))
    {
        switchCtl->m_tempCtl = data.value(key_tempCtl).toBool();
    }
    if(data.contains(key_loose))
    {
        switchCtl->m_loose = data.value(key_loose).toBool();
    }
    if(data.contains(key_ir))
    {
        switchCtl->m_ir = data.value(key_ir).toBool();
    }
    if(data.contains(key_vi))
    {
        switchCtl->m_vi = data.value(key_vi).toBool();
    }
    if(data.contains(key_identifyWaitTime))
    {
        int waitTime = data.value(key_identifyWaitTime).toInt();
        switchCtl->m_identifyWaitTime = (waitTime < 3 ? 3 : waitTime);
    }
    if(data.contains(key_idcardValue))
    {
        switchCtl->m_idcardValue = data.value(key_idcardValue).toDouble();
    }
    if(data.contains(key_screenCtl))
    {
        switchCtl->m_screenCtl = data.value(key_screenCtl).toBool();
    }

    if(data.contains(key_log))
    {
        dataShare->m_log = data.value(key_log).toBool();
    }
    if(data.contains(key_ic))
    {
        switchCtl->m_ic = data.value(key_ic).toBool();
    }



    switchCtl->saveSwitchParam();
    sendSNtoClient();
}

void ToolTcpServer::parseData(QByteArray &recData)
{
    if (!(recData.left(4)=="OFLN")) {
        return ;
    }
    QByteArray cmd;
    cmd = recData.mid(6,1);
    int m_cmd = 0;
    m_cmd = cmd[0] & 0xFF;
//    qt_debug() << m_cmd<<recData;

     if(m_cmd == Dev_CameraCalibration_request)
    {
         emit sigCamCalibration();
    }
    else
    {
        QByteArray msgBody = recData.remove(0,11);
        QJsonParseError jsonError;
        QJsonObject rootObj;
        QString msgType("");
        QString cmdStr("");

        QJsonDocument json = QJsonDocument::fromJson(msgBody, &jsonError);
        if (jsonError.error == QJsonParseError::NoError)
        {
            if(json.isObject())
            {
                rootObj = json.object();
                if(rootObj.contains("msgType"))
                {
                     msgType = rootObj["msgType"].toString();
                     cmdStr = rootObj["cmd"].toString();
                     qt_debug()<< msgType << cmdStr;
                    if(m_cmd == Dev_Information_request)
                    {
                       if(msgType == "0") //设备配置信息
                       {
                            if(cmdStr == "0") //获取设备配置信息
                            {
                                QJsonObject switchDat = switchCtl->readSwitchParam();
                                QString && dat = QString(QJsonDocument(switchDat).toJson(QJsonDocument::Compact));
                                QJsonObject jaSonObject;
                                jaSonObject.insert("msgType",msgType);
                                jaSonObject.insert("cmd",cmdStr);
                                ResponseDataToTool(Dev_Information_response,jaSonObject);

                            }else if(cmdStr == "1") //修改设备配置信息
                            {

                              if(rootObj.contains("data"))
                              {
                                  QJsonObject data = rootObj.value("data").toObject();
                                  qt_debug() << data;
                                  setParameters(data,msgType,cmdStr);
                               }
                            }
                       }
                       else if(msgType == "1")//设备硬件信息
                       {
                           if(cmdStr == "0") //获取设备硬件配置信息
                           {
                               QByteArray dat = rootObj.value("data").toString().toUtf8();
                               emit sigGetTempInfo(dat);
                           }
                       }

                    }else if(m_cmd == Dev_OfflineDatExport_request)
                    {
                            if(msgType == "0")
                            {
                                if(cmdStr == "0")//获取身份证登记信息
                                {

                                    QList<QStringList> fileList;
                                    QStringList infoList;
                                    infoList.clear();
                                    fileList.clear();
                //                    QSet<int> datas = sqlDatabase->sqlSelectSaveIdentifyAll();
                //                    if(datas.count() > 0)
                //                    {
                //                        foreach (int i, datas)
                //                        {
                //                            fileList.clear();
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(1).toString());
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(2).toString());
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(3).toString());
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(4).toString());
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(5).toString());
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(6).toString());
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(7).toString());
                //                            infoList.append(sqlDatabase->sqlSaveIdentifySelect(i).value(8).toString());
                //                            fileList.append(infoList);
                //                            infoList.clear();
                //                            sendSaveFileClient(fileList);
                //                            msleep(10);
                //                        }
                //                        msleep(100);
                //                        sendSaveEnd();
                //                    }

                                //  ResponseDataToTool(Dev_OfflineDatExport_request,msgType,cmd,"","200","");
                                }else if(cmd == "1")//删除身份证登记消息
                                {
                                    //删除数据库
                                 //   ResponseDataToTool(Dev_OfflineDatExport_request,msgType,cmd,"","200","");
                                }
                            }
                    }else if(m_cmd == Dev_FirmwareUpgrade_request)
                    {    qt_debug() << "Dev_FirmwareUpgrade_request";
                        if(msgType == "0")//恢復默认设置
                        {
                            if(cmdStr == "0")
                            {
                                switchCtl->setSwitchDefault();
                                QJsonObject jaSonObject;
                                jaSonObject.insert("msgType",msgType);
                                jaSonObject.insert("cmd",cmdStr);
                                ResponseDataToTool(Dev_FirmwareUpgrade_response,jaSonObject);
                                msleep(1000);
                                qt_debug()<<"";
                                system("rm *.db");
                                system("rm offline/*");
                                system("reboot");

                            }
                        }else if(msgType == "1")//升级固件
                        {
                             if(cmdStr == "0")
                            {
                                if(rootObj.contains("versionUpdate"))
                                {
                                    QString verStr;
                                    verStr = rootObj.value("versionUpdate").toString();
                                    QByteArray verdata = QByteArray::fromBase64(verStr.toUtf8());
                                    QFile file("F01N");
                                    if (!file.open(QFile::ReadWrite)) {
                                        qt_debug() << "open failed!";
                                        return ;
                                    }
                                    file.write(verdata);
                                    file.close();
                                    system("mv F01N F01");
                                    system("chmod 777 F01");
                                    QJsonObject response;
                                    response.insert("msgType",msgType);
                                    response.insert("cmd",cmdStr);
                                    ResponseDataToTool(Dev_FirmwareUpgrade_request,response);
                                    msleep(1000);
                                    system("reboot");
                                }
                            }
                            else if(cmdStr == "1")//
                            {   qt_debug() << "Dev_FirmwareUpgrade_request2";
                                DevUpdate(rootObj);
                            }
                            else if(cmdStr == "2")//测温固件升级
                            {
                                 if(rootObj.contains("versionTempUpdate"))
                                 {
                                     system("rm temp.bin");
                                     QString verStr;
                                     verStr = rootObj.value("versionTempUpdate").toString();
                                     QByteArray verdata = QByteArray::fromBase64(verStr.toUtf8());
                                     QFile file("temp.bin");
                                     if (!file.open(QFile::ReadWrite)) {
                                         qt_debug() << "open failed!";
                                         return ;
                                     }
                                     file.write(verdata);
                                     file.close();
                                     QJsonObject response;
                                     response.insert("msgType",msgType);
                                     response.insert("cmd",cmdStr);
                                     ResponseDataToTool(Dev_FirmwareUpgrade_request,response);
                                    // emit sigSendUpdateTemp();
                                 }
                            }
                        }
                    }else if(m_cmd == Dev_Debugging_request)
                    {
                        if(msgType == "0")//实时日志操作
                        {
                            if(cmdStr=="0")
                                emit sigRealTimeLog(true);//输出到文件
                            if(cmdStr=="1")
                                emit sigRealTimeLog(false);
                        }
                        else if(msgType == "1")//摄像头抓取图片
                        {
                           emit sigCaptureCamPicture();
                        }
                        else if(msgType == "2")//对数据库的操作
                        {
                            if(cmdStr == "0") //删除人脸
                            {
                                sqlDatabase->sqlDeleteAll();
                            }
                            else if(cmdStr == "1")//下发人脸
                            {

                                 int id = rootObj.value("id").toInt();
                                 int passNum = rootObj.value("passNum").toInt();
                                 int isBack = rootObj.value("isBack").toInt();
                                 QVariant picture = rootObj.value("picture").toInt();
                                 QString userName = rootObj.value("userName").toString();
                                 QString collectTimes = rootObj.value("collectTimes").toString();
                                 QString carNum = rootObj.value("carNum").toString();
                                 QString rightStarTime = rootObj.value("rightStarTime").toString();
                                 QString rightEndTime = rootObj.value("rightEndTime").toString();
                                 QString week = rootObj.value("week").toString();
                                 QString Times = rootObj.value("Times").toString();
                                 QString PhotoNum = rootObj.value("PhotoNum").toString();
                                 QString picName = rootObj.value("picName").toString();
                                 QString remark = rootObj.value("remark").toString();

                                 QStringList list;
                                 list << userName << collectTimes << carNum << rightStarTime << \
                                 rightEndTime << week << Times << PhotoNum << picName << remark;
                               // sqlDatabase->sqlInsert(id,passNum,isBack,picture,list);
                                // data:从索引0依次开始 用户名、采集时间、门禁卡号、权限开始时间、权限结束时间、星期字段、一天的通行时段、手机号、图片名称、备注
                                //void sqlInsert(int id, int passnum, int isBack, const QVariant &feature, const QStringList &data);
                            }
                          //  ResponseDataToTool(Dev_FirmwareUpgrade_request,msgType,cmd,"","200","");
                        } else if(msgType == "3")//重启设备
                        {
                            QJsonObject response;
                            response.insert("msgType",msgType);
                            response.insert("cmd",cmdStr);
                            ResponseDataToTool(Dev_Debugging_response,response);
                            msleep(1000);
                            system("reboot");
                        }
                    }
                    else if(m_cmd == Dev_CameraCalibration_request)
                    {
                        emit sigCamCalibration();
                    }
                    else if(m_cmd == Dev_Voice_File_request)
                    {
                        //        请求参数： 头部消息命令=0x0d
                        //        发送语音文件可以支持两种的方式，单条下发，整个数据包下发。
                        //        是否必选	参数名	类型	说明
                        //        必选	msgType	string	0下发语音文件
                        //        必选	cmd	string	为0时表示下发单个语音文件，为1时表示下发整个语言包
                        //        必选	data	qbase64	语音文件数据
                        //        必选	language	int	0中文，1英文，2日语，3韩语，4俄语，5印尼语
                        //        非必选	name	string	文件名
                        if(msgType == "0")//配置语言文件
                        {
                            if(cmdStr == "0")//下发单个语音文件
                            {
                                QString FileName = "";
                                if(rootObj.contains("language")&&rootObj.contains("name")&& rootObj.contains("data"))
                                {
                                    FileName+= "./aac"+QString::number(rootObj.value("language").toInt())+"/"+rootObj.value("name").toString();
                                    qt_debug() << FileName;
                                }else return;

                                QString fullPath;//文件夹全路径
                                fullPath = "./aac"+QString::number(rootObj.value("language").toInt());
                                qt_debug() <<fullPath;
                                QDir dir(fullPath);
                                if(!dir.exists())
                                {
                                    if(dir.mkpath(fullPath))
                                    {
                                        qt_debug() << "creat ok:" << fullPath;
                                    }else {
                                          qt_debug() << "creat path error:" << fullPath;
                                    }
                                }

                                QString verStr;
                                verStr =rootObj.value("data").toString();
                                QByteArray verdata = QByteArray::fromBase64(verStr.toUtf8());

                                QFile file(FileName);
                                if (!file.open(QFile::ReadWrite)) {
                                    qt_debug() << "open failed!";
                                    return ;
                                }

                                QJsonObject dat;
                                dat.insert("msgType",msgType);
                                dat.insert("cmd",cmdStr);
                                dat.insert("name",rootObj.value("name").toString());

                               if(file.write(verdata)>0)
                               {
                                   qt_debug() << "ok";
                                   dat.insert("state","ok");
                                }else {
                                   qt_debug() << "error";
                                   dat.insert("state","error");
                                }
                                file.close();

                                ResponseDataToTool(Dev_Voice_File_response,dat);
                            }
                            else if(cmdStr == "1")//下发整个语言包
                            {
                                VoicUpdate(rootObj);
                            }

                        }
                        else if(msgType == "1")
                        {
                            if(rootObj.contains("language"))
                            {
                                int language = rootObj.value("language").toInt();
                                switchCtl->m_language = language;
                                switchCtl->saveSwitchParam();
                                sendSNtoClient();
                            }
                        }
                        else if(msgType == "2")
                        {
                            if(rootObj.contains("tts"))
                            {
                                int tts = rootObj.value("tts").toBool();
                                switchCtl->m_tts = tts;
                                switchCtl->saveSwitchParam();
                                sendSNtoClient();
                            }
                        }
                    }else if(m_cmd == Dev_Msg_request)
                    {
                      //ResponseDataToTool(Dev_Msg_request,msgType,cmd,"","200","");
                    }
                    else if(m_cmd == Dev_TemCalibration_request)
                    {
                        if(msgType == "2")
                        {
                            if(rootObj.contains("data"))
                            {   QString data = rootObj.value("data").toString();
                                qt_debug() << data.toUtf8();
                                  emit sigGetTempInfo(data.toUtf8());
                            }

                        }
                        else if(msgType == "1")
                        {
                            emit  sigSetAllScreenOn(true);
                        }
                        else if(msgType == "0")
                        {
                             emit  sigSetAllScreenOn(false);
                        }
                        else if(msgType == "3")//获取测温模块的信息
                        {
                            qt_debug() << "sigGetTempHardwareInfo";
                             emit  sigGetTempHardwareInfo();
                            msleep(200);

                            QByteArray sendData = "";
                            QJsonObject dat ;
                            QJsonObject result ;

                            dat.insert("tempOffset",QString::number(dataShare->m_offset));
                            dat.insert("tempType",dataShare->m_tempType);
                            dat.insert("tempVer",dataShare->m_tempVer);
                            dat.insert("tempOffdata",QString::number(dataShare->m_offdata));
                            dat.insert("tempDevice",dataShare->m_tempDevice);
                            result.insert("msgType","3");
                            result.insert("data",dat);

                            QJsonDocument document;

                            document.setObject(result);
                            QByteArray stateData = document.toJson(QJsonDocument::Compact);
                            sendData.append("OFLN");
                            sendData.append(uchar(1));
                            sendData.append(uchar(1));
                            sendData.append(uchar(Dev_TemCalibration_response));
                            sendData.append(intToByte(stateData.length()));
                            sendData.append(stateData);
                            qt_debug() << sendData ;
                            m_tcpSocket->write(sendData);
                            m_tcpSocket->flush();

                        }
                    }

                }
             }
        }
    }
    system("sync");
    dataSize = 0;
    dataArray.clear();    
}

void ToolTcpServer::sendSaveEnd()
{
    QByteArray sendData = "";
    QJsonObject jsonObj;
    QJsonObject dataObj;
    jsonObj.insert("readSaveEnd", "end");
    QJsonDocument document;
    document.setObject(jsonObj);
    QByteArray stateData = document.toJson(QJsonDocument::Compact);
    sendData.append("OFLN");
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(intToByte(stateData.length()));
    sendData.append(stateData);
    m_tcpSocket->write(sendData);
    m_tcpSocket->flush();
}

void ToolTcpServer::sendSaveFileClient(QList<QStringList> fileList)
{
    qt_debug()<<fileList.count();
    if(fileList.count() > 0)
    {
        QByteArray sendData = "";
        QJsonObject jsonObj;
        QJsonArray fileArray;
        QJsonObject dataObj;
        foreach(QStringList k, fileList)
        {
            dataObj.insert("num",k.at(0));
            dataObj.insert("name", k.at(1));
            dataObj.insert("sex", k.at(2));
            dataObj.insert("nation", k.at(3));
            dataObj.insert("birth", k.at(4));
            dataObj.insert("temp", k.at(5));
            QFile file(k.at(6));
            file.open(QIODevice::ReadWrite);
            QString snaps = QString::fromUtf8(file.readAll().toBase64());
            file.close();
            dataObj.insert("image", snaps);
            dataObj.insert("snaptime", k.at(7));
            fileArray.append(dataObj);
        }
        jsonObj.insert("readSaveIdentifyFile", fileArray);
        QJsonDocument document;
        document.setObject(jsonObj);
        QByteArray stateData = document.toJson(QJsonDocument::Compact);
        sendData.append("OFLN");
        sendData.append(uchar(1));
        sendData.append(uchar(1));
        sendData.append(uchar(Dev_OfflineDatExport_response));
        sendData.append(intToByte(stateData.length()));
        sendData.append(stateData);
        m_tcpSocket->write(sendData);
        m_tcpSocket->flush();
    }
}


void ToolTcpServer::responseClient(QString state)
{
    qt_debug()<<__PRETTY_FUNCTION__<<state;
    QByteArray sendData = "";
    QJsonObject sendObj;
    sendObj.insert("state",state);
    QJsonDocument document;
    document.setObject(sendObj);
    QByteArray stateData = document.toJson(QJsonDocument::Compact);
    sendData.append("OFLN");
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(intToByte(stateData.length()));
    sendData.append(stateData);
    m_tcpSocket->write(sendData);
    m_tcpSocket->flush();
}

void ToolTcpServer::sendSNtoClient()
{
    QByteArray sendData = "";
    QJsonObject dat = switchCtl->readSwitchParam();
    dat.insert("log",dataShare->m_log);
    dat.insert("sn",switchCtl->m_sn);
    dat.insert("screenCtl",switchCtl->m_screenCtl);

    dat.insert("msgType","0");
    dat.insert("cmd","0");
    dat.insert("data",dat);

    QJsonDocument document;
    document.setObject(dat);
    QByteArray stateData = document.toJson(QJsonDocument::Compact);
    sendData.append("OFLN");
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(Dev_Information_response));
    sendData.append(intToByte(stateData.length()));
    sendData.append(stateData);
    qt_debug() << sendData ;
    m_tcpSocket->write(sendData);
    m_tcpSocket->flush();
}

void ToolTcpServer::ResponseDataToTool(ToolCmdHead headCmd,QJsonObject & sendObj)
{
    QByteArray sendData = "";
    QJsonDocument document;
    document.setObject(sendObj);
    QByteArray stateData = document.toJson(QJsonDocument::Compact);
    sendData.append("OFLN");
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(headCmd));
    sendData.append(intToByte(stateData.length()));
    sendData.append(stateData);
    m_tcpSocket->write(sendData);
    m_tcpSocket->flush();
}

void ToolTcpServer::DevUpdate(QJsonObject rootObj)
{
    QJsonObject dataObj;
    dataObj = rootObj.value("hardUpdate").toObject();
    if(dataObj.contains("hardUpdate"))
    {
        dataShare->m_upgrade = true;
        QString verStr;
        if(dataObj.contains("number"))
        {
            verStr = dataObj.value("hardUpdate").toString();
            if(dataObj.value("number").toInt() != 1)
            {
//                            real_verStr = real_verStr + verStr;
                QFile saveFile("base64SaveFile.txt");
                if(saveFile.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    qt_debug() << verStr.length();
                    saveFile.write(verStr.toUtf8());
//                                real_verStr = "";
                }
                saveFile.close();
                responseHardUpdate(Dev_FirmwareUpgrade_response,"Success");
            }
            else
            {
                QFile saveFile("base64SaveFile.txt");
                if(saveFile.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    qt_debug();
                    saveFile.write(verStr.toUtf8());
                }
                saveFile.close();

                QFile readFile("base64SaveFile.txt");
                QByteArray m_save;
                if(readFile.open(QIODevice::ReadWrite))
                {
                    qt_debug();
                    m_save = readFile.readAll();
                }
                readFile.close();

                QFile file("update.tar.xz");
                if(file.open(QIODevice::ReadWrite))
                {
                    //QByteArray verdata = m_save;
                    file.write(QByteArray::fromBase64(m_save));
                }
                file.close();
                if(dataObj.contains("md5"))
                {
                    responseHardUpdate(Dev_FirmwareUpgrade_response,"Success");
                    qt_debug();
                    QFile tarfile("update.tar.xz");
                    QByteArray data;
                    if(tarfile.open(QIODevice::ReadWrite))
                    {
                        //QByteArray verdata = m_save;
                        data = tarfile.readAll();
                    }
                    tarfile.close();
                    QString md5 = QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
                    qt_debug() << md5 << dataObj.value("md5").toString();
                    if(md5 == dataObj.value("md5").toString())
                    {
                        responseHardUpdate(Dev_FirmwareUpgrade_response,"tarxz");
                        system("tar -xvf update.tar.xz && rm update.tar.xz");
                        system("rm base64SaveFile.txt");
                        if (QFile::exists("hi3516dv300_smp_image")) {
                            char* mac ;
                            mac = switchCtl->m_sn.toUtf8().data();
                             if (mac[7] >= 'A') {
                                mac[7] -= '7';
                                mac[7] &= 0xE;
                                mac[7] += '7';
                            } else {
                                mac[7] -= '0';
                                mac[7] &= 0xE;
                                mac[7] += '0';
                            }
                            system("echo " + QByteArray(mac) + " > /dev/mmcblk0p6");
                            system("dd if=hi3516dv300_smp_image/u-boot-hi3516dv300.bin of=/dev/mmcblk0p1 &&"
                                   "dd if=hi3516dv300_smp_image/uImage_hi3516dv300_smp of=/dev/mmcblk0p2 &&"
                                   "rm hi3516dv300_smp_image -rf &&"
                                   "sync");
                        }

                        responseHardUpdate(Dev_FirmwareUpgrade_response,"reboot");
                        msleep(1000);
                        qt_debug() << "system reboot";
                        system("reboot");
                    }
                    else {
                        system("rm base64SaveFile.txt");
                        system("rm update.tar.xz");
                        responseHardUpdate(Dev_FirmwareUpgrade_response,"error");
                    }
                    dataShare->m_upgrade = false;
                }
            }
        }

    }

}


void ToolTcpServer::VoicUpdate(QJsonObject rootObj)
{
    QJsonObject dataObj;
    dataObj = rootObj.value("hardUpdate").toObject();
    if(dataObj.contains("hardUpdate"))
    {
        QString verStr;
        if(dataObj.contains("number"))
        {
            verStr = dataObj.value("hardUpdate").toString();
            if(dataObj.value("number").toInt() != 1)
            {
                QFile saveFile("voiceBase64SaveFile.txt");
                if(saveFile.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    qt_debug() << verStr.length();
                    saveFile.write(verStr.toUtf8());
                }
                saveFile.close();
               // responseHardUpdate(Dev_Voice_File_response,"Success");
            }
            else
            {
                QFile saveFile("voiceBase64SaveFile.txt");
                if(saveFile.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    qt_debug();
                    saveFile.write(verStr.toUtf8());
                }
                saveFile.close();

                QFile readFile("voiceBase64SaveFile.txt");
                QByteArray m_save;
                if(readFile.open(QIODevice::ReadWrite))
                {
                    qt_debug();
                    m_save = readFile.readAll();
                }
                readFile.close();

                QFile file("Voiceupdate.tar.xz");
                if(file.open(QIODevice::ReadWrite))
                {
                    file.write(QByteArray::fromBase64(m_save));
                }
                file.close();
                if(dataObj.contains("md5"))
                {

                    QJsonObject sendObj;
                    sendObj.insert("msgType","0");
                    sendObj.insert("cmd","1");

                    sendObj.insert("state","saveFile");
                    responseDataToService(Dev_Voice_File_response,sendObj);

                    qt_debug();
                    QFile tarfile("Voiceupdate.tar.xz");
                    QByteArray data;
                    if(tarfile.open(QIODevice::ReadWrite))
                    {
                        data = tarfile.readAll();
                    }
                    tarfile.close();
                    QString md5 = QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
                    qt_debug() << md5 << dataObj.value("md5").toString();
                    if(md5 == dataObj.value("md5").toString())
                    {
                        sendObj.insert("state","tarxz");
                        responseDataToService(Dev_Voice_File_response,sendObj);

                        system("tar -xvf Voiceupdate.tar.xz && rm Voiceupdate.tar.xz");
                        system("rm voiceBase64SaveFile.txt");

                        sendObj.insert("state","ok");
                        responseDataToService(Dev_Voice_File_response,sendObj);
                        msleep(1000);
                        qt_debug() << "total voice file ok";

                    }
                    else {
                        system("rm voiceBase64SaveFile.txt");
                        system("rm Voiceupdate.tar.xz");

                        sendObj.insert("state","error");
                        responseDataToService(Dev_Voice_File_response,sendObj);
                    }
                }
            }
        }

    }

}

void ToolTcpServer::responseDataToService(ToolCmdHead cmd, QJsonObject & sendObj)
{
    QByteArray sendData = "";
    QJsonDocument document;
    document.setObject(sendObj);
    QByteArray stateData = document.toJson(QJsonDocument::Compact);
    sendData.append("OFLN");
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(cmd));
    sendData.append(intToByte(stateData.length()));
    sendData.append(stateData);
    m_tcpSocket->write(sendData);
    m_tcpSocket->flush();
}

void ToolTcpServer::responseHardUpdate(ToolCmdHead cmd,QString state)
{

    QByteArray sendData = "";
    QJsonObject sendObj;
    sendObj.insert("result","200");
    sendObj.insert("desc","");
    sendObj.insert("data",state);
    sendObj.insert("msgType","1");
    sendObj.insert("cmd","1");
    QJsonDocument document;
    document.setObject(sendObj);
    QByteArray stateData = document.toJson(QJsonDocument::Compact);
    sendData.append("OFLN");
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(cmd));
    sendData.append(intToByte(stateData.length()));
    sendData.append(stateData);
    m_tcpSocket->write(sendData);
    m_tcpSocket->flush();
}

