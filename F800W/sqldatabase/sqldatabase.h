#ifndef SQLDATABASE_H
#define SQLDATABASE_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QMutex>
#include <QDebug>

#define sqlDatabase SqlDatabase::getInstance()

class SqlDatabase : public QObject
{
    Q_OBJECT
public:
    explicit SqlDatabase();
    static SqlDatabase *getInstance()
    {
        if (!m_Instance) {
            m_Instance = new SqlDatabase();
        }
        return m_Instance;
    }
    // 当前所有人脸id
    QSet<int> m_localFaceSet;
    // 查询所有userid;
    void sqlSelectAllUserId();
    // 根据人员id查询表中对应人员的其他信息
    QVariantList sqlSelect(int id);
    // 根据人员id查询表中对应人员的采集图片名称
    QString sqlSelectPhotoName(int id);
    // 人员信息插入表格, id:唯一的人员mids,passnum:开门权限通行次数，isBack:是否白名单， feature:人脸特征值
    // data:从索引0依次开始 用户名、采集时间、门禁卡号、权限开始时间、权限结束时间、星期字段、一天的通行时段、手机号、图片名称、备注
    void sqlInsert(int id, int passnum, int isBack, const QVariant &feature, const QStringList &data);
    // 根据人员id删除人员
    void sqlDelete(int id);
    // 删除所有人员信息
    void sqlDeleteAll();
    // 人员信息插入表格, id:唯一的人员mids,passnum:开门权限通行次数，isBack:是否白名单， feature:人脸特征值
    // data:从索引0依次开始 用户名、采集时间、门禁卡号、权限开始时间、权限结束时间、星期字段、一天的通行时段、手机号、图片名称、备注
    void sqlUpdate(int id, int passnum, int isBack, const QStringList &data);
    // 更新开门权限开门次数
    void sqlUpdatePass(int id, int passnum);
    // 获取人员图片特征值
    QString sqlGetFeature(int id);

private:
    static SqlDatabase *m_Instance;
    QSqlDatabase m_database;
    QMutex m_mutex;
};
#endif // SQLDATABASE_H
