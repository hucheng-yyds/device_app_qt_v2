#ifndef SQLDATABASE_H
#define SQLDATABASE_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include "FaceIDAPI.h"
#include "log.h"
#include "datashare.h"

#define sqlDatabase SqlDatabase::getInstance()

class SqlDatabase : public QObject
{
    Q_OBJECT
signals:
    void removeFaceGroup(int id);

public:
    explicit SqlDatabase();
    static SqlDatabase *getInstance()
    {
        if (!m_Instance) {
            m_Instance = new SqlDatabase();
        }
        return m_Instance;
    }
    // 失败入库id
    QSet<int> m_localFaceFail;
    // 当前所有人脸id
    QSet<int> m_localFaceSet;
    // 查询所有userid;
    QSet<int> sqlSelectAllUserId();
    // 查询所有user特征值;
    QString sqlSelectAllUserFeature(int id);
    // 查询时间戳;
    QString sqlSelectAllUserTime(int id);
    // 根据人员id查询表中对应人员的其他信息
    QVariantList sqlSelect(int id);
    // 根据人员id查询表中对应人员的采集图片名称
    QString sqlSelectPhotoName(int id);
    // 人员信息插入表格
    void sqlInsert(int id, const QString &username, const QString &time, const QString &feature, const QString &photoname, const QString &iphone);
    // 根据人员id删除人员
    void sqlDelete(int id);
    // 删除所有人员信息
    void sqlDeleteAll();
    // 人员信息更新
    void sqlUpdate(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone);

    // 离线记录数据插入
   void sqlInsertOffline(int id, int userid, int type, int isOver, int isTemp, int sex, const QStringList &datas);
    // 查询所有离线记录id
    QList<int> sqlSelectAllOffLine();
    // 离线记录查询
    QVariantList sqlSelectOffline(int id);
    // 根据人员id 删除对应离线记录
    void sqlDeleteOffline(int id);
    // 删除所有离线记录
    void sqlDeleteAllOffline();

    // 根据失败id查询对应内容
    int sqlSelectFaile(int id);
    // 插入失败记录
    void sqlInsertFail(int id, int type);
    // 根据id删除对应记录
    void sqlDeleteFail(int id);
    // 删除所有失败记录
    void sqlDeleteAllFail();

    // 人员权限插入
    void sqlInsertAuth(int id, int passNum, int isBlack, const QStringList &datas);
    // 人员权限更新
    void sqlUpdateAuth(int id, int passNum, int isBlack, const QStringList &datas);
    // 根据人员id权限
    void sqlDeleteAuth(int id);
    // 删除所有人员权限
    void sqlDeleteAllAuth();
    // 更新通行次数
    void sqlUpdatePassNum(int id, int passNum);
    // 根据人员id查询权限
    QVariantList sqlSelectAuth(int id);

    // ic卡数据插入
    void sqlInsertIc(int id, const QString &cardNo);
    // ic卡数据内容更新
    void sqlUpdateIc(int id, const QString &cardNo);
    // 根据人员id进行ic卡数据删除
    void sqlDeleteIc(int id);
    // 删除所有人员IC卡数据
    void sqlDeleteAllIc();
    // 根据人员id查询IC卡内容
    QString sqlSelectIc(int id);
    // 根据IC卡号查询人员id
    int sqlSelectIcId(const QString &cardNo);

private:
    static SqlDatabase *m_Instance;
    QSqlDatabase m_database;
    QMutex m_mutex;
};
#endif // SQLDATABASE_H
