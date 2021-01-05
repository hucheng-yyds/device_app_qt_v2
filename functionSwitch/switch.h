#ifndef SWITCH_H
#define SWITCH_H
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include "log.h"

#define switchCtl SwitchCtl::getInstance()

#define VERSION     "1.0.0.002"
#define DEVICE_TYPE "F700B"

class SwitchCtl
{
public:
    explicit SwitchCtl();
    ~SwitchCtl();
    static SwitchCtl *getInstance()
    {
        if (!m_Instance) {
            m_Instance = new SwitchCtl();
        }
        return m_Instance;
    }
    // 保存屏的参数
    void saveSreenParam();
    // 读取屏的参数
    QJsonObject readScreenParam();
    // 保存开关量参数
    void saveSwitchParam();
    // 读取开关量参数
    QJsonObject readSwitchParam();
    // 恢复开关量默认值
    void setSwitchDefault();

    // 设备sn码
    QString m_sn;
    // 旋转角度，1是90度，3是270度
    int m_angle;
    // 屏的参数 0:5.5寸屏 3和4：8寸屏 9：10寸屏
    int m_screen;
    // 摄像头参数 一般不变
    int m_camera;
    // 门禁开关
    bool m_faceDoorCtl;
    // 测温开关
    bool m_tempCtl;
    // 宽松模式开关 true:打开宽松模式 false:关闭宽松模式
    bool m_loose;
    // 活体检测开关
    bool m_ir;
    // 访客模式开关
    bool m_vi;

    // 人脸比对等待时间间隔 单位秒
    int m_identifyWaitTime;
    // 人脸和身份证比对阈值
    double m_idcardValue;

    // 自动获取ip和手动配置ip开关 true自动获取，false手动配置
    bool m_ipMode;
    // 手动配置模式ip
    QString m_manualIp;
    // 手动配置模式网关
    QString m_manualGateway;
    // 手动配置模式子网掩码
    QString m_manualNetmask;
    // 手动配置模式dns
    QString m_manualDns;

    // 息屏模式开关 true:常亮 false:定时息屏
    bool m_screenCtl;
    // 定时息屏时间 单位秒最小3秒钟
    int m_closeScreenTime;

    // 后台通信协议开关 1:tcp协议，2：中间件协议，3:http协议
    int m_protocol;
    // tcp后台地址
    QString m_tcpAddr;
    // tcp后台端口号
    int m_tcpPort;
    // http后台访问地址
    QString m_httpAddr;
    // tcp后台请求超时时间
    int m_tcpTimeout;
    // ntp始终服务器地址
    QString m_ntpAddr;

    // 温度补偿值
    double m_tempComp;
    // 测温告警值
    double m_warnValue;

    // 时区
    QString m_timeZone;
    // 开门条件
    QString m_openMode;
    // 识别距离 0:检测的最远距离（与算法和摄像头相关） 1：1米 2:2米
    int m_identifyDistance;
    // 开门等待时间 单位秒
    int m_doorDelayTime;
    // 安全帽开关
    int m_helet;
    // 口罩开关 0:表示关闭 1:提醒 2:检测
    int m_mask;
    // 显示ic卡号 开关
    bool m_showIc;
    // 华氏摄氏度开关 true:开华氏 false:关华氏
    bool m_fahrenheit;
    // 红外灯常亮控制 格式： 02:00-06:00|18:00-20:00|23:00-23:59
    QString m_irLightCtl;
    // 补光灯常亮控制 格式： 02:00-06:00|18:00-20:00|23:00-23:59
    QString m_bgrLightCtl;
    // 上传记录是否带图片 true:带图片 false:不带图片
    bool m_uploadImageCtl;
    // 是否上传陌生人记录 true:上传 false不上传
    bool m_uploadStrangerCtl;
    // 语言切换 0:中文 1:英文 2:日语 3:韩语 4:俄罗斯语 5:印尼语
    int m_language;
    // 设备名称
    QString m_devName;
    // 姓名脱敏 0:不脱敏 1:姓脱敏 2:姓名显示您好
    int m_nameMask;
    // tts开关 true:打开 false:关闭
    bool m_tts;
    // 温度值播报数字开关，只有当tts打开才有效
    bool m_tempValueBroadcast;
    // 二维码开关 0:关闭 1:摄像头识别 2：读头识别
    int m_rcode;
    // 音量控制 0 - 100 0是静音
    int m_volume;
    // ic卡开关
    bool m_ic;
    // tcp后台登录的密码
    QString m_passwd;
    // 星云秘钥
    QString m_appScret;

    // wifi模式
    bool m_wifiCtl;
    // wifi用户名
    QString m_wifiName;
    // wifi密码
    QString m_wifiPwd;

private:
    // 屏参和摄像头参数 参数不要随意修改
    void setScreenDefault();

private:
    static SwitchCtl *m_Instance;
};
#endif // SWITCH_H
