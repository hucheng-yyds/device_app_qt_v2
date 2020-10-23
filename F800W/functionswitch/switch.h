#ifndef SWITCH_H
#define SWITCH_H
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include "qalhardware.h"
#include "sqldatabase.h"

#define hardware    QalHardWare::getInstance()
#define switchCtl SwitchCtl::getInstance()
#define sqlDatabase SqlDatabase::getInstance()

class SwitchCtl
{
public:
    explicit SwitchCtl();
    static SwitchCtl *getInstance()
    {
        if (!m_Instance) {
            m_Instance = new SwitchCtl();
        }
        return m_Instance;
    }
    // 保存屏的参数
    void saveSreenParam(const QJsonObject &obj);
    // 读取屏的参数
    QJsonObject readScreenParam();
    // 保存开关量参数
    void saveSwitchParam(const QJsonObject &obj);
    // 读取开关量参数
    QJsonObject readSwitchParam();

private:
    void setSwitchDefault();
    void setScreenDefault();

private:
    static SwitchCtl *m_Instance;
};
#endif // SWITCH_H
