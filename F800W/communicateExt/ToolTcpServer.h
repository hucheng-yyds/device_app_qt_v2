#ifndef TOOLTCPSERVER_H
#define TOOLTCPSERVER_H

#include <QObject>
#include <QtNetwork>
#include <QTcpServer>
#include "UdpServer.h"
#include "sample_vio.h"

#define ZK_SFZ_OFFLINE




typedef enum {

    Dev_Information_request = 1,//0x01
    Dev_Information_response ,

    Dev_TemCalibration_request,
    Dev_TemCalibration_response,

    Dev_OfflineDatExport_request,
    Dev_OfflineDatExport_response,

    Dev_FirmwareUpgrade_request,
    Dev_FirmwareUpgrade_response,

    Dev_Debugging_request,
    Dev_Debugging_response,

    Dev_Voice_File_request,
    Dev_Voice_File_response,

    Dev_CameraCalibration_request,
    Dev_CameraCalibration_response,

    Dev_HeartBeat_request,
    Dev_HeartBeat_response,

    Dev_Msg_request = 48,
    Dev_Msg_response = 49
 }ToolCmdHead;

#define HEART_BEART_TIME 6000

class ToolTcpServer : public QThread
{
    Q_OBJECT
public:
    explicit ToolTcpServer();
signals:
    void sigCaptureCamPicture();//摄像头抓拍图片
    void sigCamCalibration();//摄像头校准
    void sigGetTempInfo(QByteArray tempInfo);//测温模块,tempInfo为uchar型命令
    void sigGetTempHardwareInfo();//获取测温模块的信息
    void sigRealTimeLog(bool);//控制日志模块
    void sigToolTcpStateChange(bool state);//true:链接上了，false:链接断开
    void sigSetAllScreenOn(bool);//打開或關閉全屏測溫
public slots:
//    void onToolCmdResponse(ToolCmds cmd ,QByteArray dat);//其他的模块通过这个接口反馈消息到配置工具
    void onGetTempResponse(QByteArray dat);//测温模块的反馈接口
    void onGetTempHardwareInfo(QJsonObject );//通过此接口返回测温模块的硬件信息，返回字段如下:Temp
    void onCaptureCamPicture(QByteArray dat);//抓取到图片的数据
    void onCamCalibration();//摄像头校准的结果
    void onGetRealTimeLog(QString);//日志模块输入信息
private slots:
      void onNewConnect(void);
      void onTcpRead(void);
      void OnStateChanged(QAbstractSocket::SocketState state);
private:
    QTcpServer   *m_tcpServer = nullptr;
    QTcpSocket   *m_tcpSocket = nullptr;
    UdpServer    m_udpServer;
    void sendHeartBeat(void);//發送心跳
    void get1080pImage();
    void sendImage();

    QTimer * m_heartBeat = nullptr;
    bool m_isSendingLostsOfData = false;//在發送大數據,暫停心跳檢測,true:暫停檢測，false:檢測心跳

     int         mTool_msgLen    = 0;
     QByteArray  mTool_msgBody   = "";

//    int dataSize;
//    QByteArray dataArray;
    void parseData(QByteArray &recData);
    void setParameters(QJsonObject & data,QString msgType,QString cmdStr);
    //response
    void ResponseDataToTool(ToolCmdHead headCmd,QJsonObject & sendObj);
    void sendSNtoClient(void);
    void responseClient(QString state);
    void onTempInfo(QByteArray tempInfo);
    void sendSaveFileClient(QList<QStringList> fileList);
    void sendSaveEnd();
    void DevUpdate(QJsonObject rootDat);
    void VoicUpdate(QJsonObject rootObj);
    void responseHardUpdate(ToolCmdHead cmd,QString state);//返回數據到配置工具
    void responseDataToService(ToolCmdHead cmd,QJsonObject &sendObj);//返回數據到配置工具
    void setSendingLostsOfData(bool flag);
};

#endif // TOOLTCPSERVER_H
