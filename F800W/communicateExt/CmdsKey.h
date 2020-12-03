#ifndef CMDSKEY_H
#define CMDSKEY_H

#define key_ntpAddr                         "ntpAddr"               // ntp地址
#define key_httpAddr                        "httpAddr"              // http后台访问地址
#define key_server_ip                       "tcpAddr"               // tcp地址
#define key_server_port                     "tcpPort"               // tcp端口
#define key_protocol                        "protocol"              // 后台通信协议开关 true:tcp协议，false:http协议

#define key_manual_config                   "ipMode"                // 自动获取ip和手动配置ip开关
#define key_manual_ip                       "manualIp"
#define key_manual_netmask                  "manualGateway"
#define key_manual_route                    "manualNetmask"
#define key_manual_dns                      "manualDns"

#define key_closeScreenTime                 "closeScreenTime"       // 定时息屏时间 单位秒最小3秒钟
#define key_openMode                        "openMode"              // 开门条件
#define key_identifyDistance                "identifyDistance"      // 识别距离
#define key_doorDelayTime                   "doorDelayTime"         // 开门等待时间 单位秒
#define key_helet                           "helet"                 // 安全帽开关
#define key_mask                            "mask"                  // 口罩开关
#define key_showIc                          "showIc"                // 显示ic卡号 开关
#define key_fahrenheit                      "fahrenheit"            // 华氏摄氏度开关 true:开华氏 false:关华氏
#define key_irLightCtl                      "irLightCt"             // 红外灯常亮控制 格式： 02:00-06:00|18:00-20:00|23:00-23:59
#define key_bgrLightCtl                     "bgrLightCtl"           // 补光灯常亮控制 格式： 02:00-06:00|18:00-20:00|23:00-23:59
#define key_uploadImageCtl                  "uploadImageCtl"        // 上传记录是否带图片 true:带图片 false:不带图片
#define key_uploadStrangerCtl               "uploadStrangerCtl"     // 是否上传陌生人记录 true:上传 false不上传
#define key_language                        "language"              // 语言切换 0:中文 1:英文 2:日语 3:韩语 4:俄罗斯语 5:印尼语
#define key_devName                         "devName"               // 设备名称
#define key_nameMask                        "nameMask"              // 姓名脱敏 0:不脱敏 1:姓脱敏 2:姓名显示您好
#define key_tts                             "tts"                   // tts开关 true:打开 false:关闭
#define key_tempValueBroadcast              "tempValueBroadcast"    // 温度值播报数字开关，只有当tts打开才有效
#define key_rcode                           "rcode"                 // 二维码开关
#define key_volume                          "volume"                // 音量控制
#define key_temp_warnValue                  "warnValue"             // 测温告警值
#define key_temp_comp                       "tempComp"              // 温度补偿值
#define key_log                             "log"              // 温度补偿值
#define key_ic                              "ic"                    // ic卡

#define key_faceDoorCtl                     "faceDoorCtl"           // 门禁开关
#define key_tempCtl                         "tempCtl"               // 测温开关
#define key_loose                           "loose"                 // 宽松模式开关 true:打开宽松模式 false:关闭宽松模式
#define key_ir                              "ir"                    // 活体检测开关
#define key_vi                              "vi"                    // 访客模式开关

#define key_identifyWaitTime                "identifyWaitTime"      // 人脸比对等待时间间隔 单位秒
#define key_idcardValue                     "idcardValue"           // 人脸和身份证比对阈值
#define key_screenCtl                       "screenCtl"             // 息屏模式开关 true:常亮 false:定时息屏



#define key_wifi_name                       "wifiName"
#define key_wifi_pwd                        "wifiPwd"

#endif // CMDSKEY_H
