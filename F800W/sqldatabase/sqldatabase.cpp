#include "sqldatabase.h"

SqlDatabase* SqlDatabase::m_Instance = nullptr;

SqlDatabase::SqlDatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        qDebug() << "contains qt_sql_default_connection";
        m_database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName("facedatas.db");
        if (!m_database.open())
        {
            qDebug() << "Error: Failed to connect database." << m_database.lastError();
        }
        else
        {
            qDebug() << "Succeed to connect database." ;
        }
    }

    QSqlQuery query(m_database);
    if (!query.exec("create table userdata ("
                    "id int primary key,"
                    "username text,"
                    "edittime text,"
                    "feature text,"
                    "photoname text,"
                    "iphone text)")) {
        qDebug() << query.lastError();
    }

    QSqlQuery query1(m_database);
    if (!query1.exec("create table offline ("
                    "id INTEGER primary key AUTOINCREMENT,"
                    "oId int,"
                    "userId int,"
                    "unlockType int,"
                    "unlockTime text,"
                    "temperature text,"
                    "isOver int,"
                    "isTemp int,"
                    "isSuccess text,"
                    "isStranger text,"
                    "reason text,"
                    "cardNo text)")) {
        qDebug() << query1.lastError();
    }
    QSqlQuery query2(m_database);
    if (!query2.exec("create table insertFail ("
                    "id int primary key,"
                    "type int)")) {
        qDebug() << query2.lastError();
    }

    QSqlQuery query3(m_database);
    if (!query3.exec("create table SaveIdentify ("
                     "id int primary key,"
                     "userid text,"
                     "username text,"
                     "usersex text,"
                     "usernation text,"
                     "usertime text,"
                     "temp text,"
                     "path text,"
                     "snaptime text)")) {
        qDebug() << query3.lastError();
    }
    QSqlQuery query4(m_database);
    if (!query3.exec("create table auth ("
                     "id int primary key,"
                     "passNum int,"
                     "startTime text,"
                     "expireTime text,"
                     "isBlack int,"
                     "passPeriod text,"
                     "passTimeSection text,"
                     "remark text)")) {
        qDebug() << query3.lastError();
    }
    QSqlQuery query5(m_database);
    if (!query3.exec("create table iccard ("
                     "id int primary key,"
                     "cardNo text)")) {
        qDebug() << query3.lastError();
    }
}


QString SqlDatabase::sqlSelectPhotoName(int id)
{
    m_mutex.lock();
    QString value = "";
    QSqlQuery query(m_database);
    QString query_sql = "select photoname from userdata where id = ?";
    query.prepare(query_sql);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << query.lastError();
    } else {
        while(query.next()) {
            value = query.value(0).toString();
        }
    }
    m_mutex.unlock();
    return value;
}

QVariantList SqlDatabase::sqlSelect(int id)
{
    m_mutex.lock();
    QVariantList value;
    QSqlQuery query(m_database);
    QString query_sql = "select * from userdata where id = ?";
    query.prepare(query_sql);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << query.lastError();
    } else {
        while(query.next()) {
            value << query.value(0) << query.value(1) << query.value(2) << query.value(3) << query.value(4) << query.value(5);
        }
    }
    m_mutex.unlock();
    return value;
}

QSet<int> SqlDatabase::sqlSelectAllUserId()
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString query_sql = "select id from userdata";
    query.prepare(query_sql);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            m_localFaceSet.insert(query.value(0).toInt());
        }
    }
    m_mutex.unlock();
    return m_localFaceSet;
}

QString SqlDatabase::sqlSelectAllUserTime(int id)
{
    m_mutex.lock();
    QString time = "";
    QSqlQuery query(m_database);
    QString query_sql = "select edittime from userdata where id = ?";
    query.prepare(query_sql);
    query.addBindValue(id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            time = query.value(0).toString();
        }
    }
    m_mutex.unlock();
    return time;
}

QString SqlDatabase::sqlSelectAllUserFeature(int id)
{
    m_mutex.lock();
    QString feature = "";
    QSqlQuery query(m_database);
    QString query_sql = "select feature from userdata where id = ?";
    query.prepare(query_sql);
    query.addBindValue(id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            feature = query.value(0).toString();
        }
    }
    m_mutex.unlock();
    return feature;
}

void SqlDatabase::sqlInsert(int id, const QString &username, const QString &time, const QString &feature, const QString &photoname, const QString &iphone)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString insert_sql = "insert into userdata values (?, ?, ?, ?, ?, ?)";
    query.prepare(insert_sql);
    query.addBindValue(id);
    query.addBindValue(username);
    query.addBindValue(time);
    query.addBindValue(feature);
    query.addBindValue(photoname);
    query.addBindValue(iphone);
    if (!query.exec())
    {
        qDebug() << query.lastError() << id;
    }
    m_localFaceSet.insert(id);
    m_mutex.unlock();
}


void SqlDatabase::sqlUpdate(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString update_sql = "update userdata set username = :username, edittime = :edittime, photoname = :photoname, iphone = :iphone "
                         "where id = :id";
    query.prepare(update_sql);
    query.bindValue(":username", username);
    query.bindValue(":edittime", time);
    query.bindValue(":photoname", photoname);
    query.bindValue(":iphone", iphone);
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlDeleteAll()
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString delete_sql = "delete from userdata";
    query.prepare(delete_sql);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    qDebug() << "clear all face";
    m_localFaceSet.clear();
    resetFaceGroup(m_groupHandle);
    m_mutex.unlock();
}

void SqlDatabase::sqlDelete(int id)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString delete_sql = "delete from userdata where id = ?";
    query.prepare(delete_sql);
    query.addBindValue(id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    removeFaceGroup(m_groupHandle, id);
    m_localFaceSet.remove(id);
    m_mutex.unlock();
}

void SqlDatabase::sqlInsertOffline(int id, int userid, int type, int isOver, int isTemp, const QStringList &datas)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString cmd = QString("insert into offline values(NULL,%1,%2,%3,'%4','%5',%6,%7,'%8','%9','%10','%11')").arg(id).arg(userid).arg(type)
            .arg(datas.at(0)).arg(datas.at(1)).arg(isOver).arg(isTemp).arg(datas.at(2)).arg(datas.at(3)).arg(datas.at(4)).arg(datas.at(5));
    if (!query.exec(cmd))
    {
        qDebug() << query.lastError() << datas;
    }
    m_mutex.unlock();
}

QList<int> SqlDatabase::sqlSelectAllOffLine()
{
    m_mutex.lock();
    QList<int> values;
    QSqlQuery query1(m_database);
    QString query_sql = "select userId from offline";
    query1.prepare(query_sql);
    if (!query1.exec())
    {
        qDebug() << query1.lastError();
    }
    else
    {
        while(query1.next())
        {
            values << query1.value(0).toInt();
        }
    }
    m_mutex.unlock();
    return values;
}

QVariantList SqlDatabase::sqlSelectOffline(int userid)
{
    m_mutex.lock();
    QVariantList values;
    QSqlQuery query(m_database);
    QString query_sql = "select * from offline where userId = ?";
    query.prepare(query_sql);
    query.addBindValue(userid);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            values << query.value(0) << query.value(1) << query.value(2) << query.value(3) << query.value(4)
                  << query.value(5) << query.value(6) << query.value(7) << query.value(8) << query.value(9)
                  << query.value(10) << query.value(11) << query.value(12);
        }
    }
    m_mutex.unlock();
    return values;
}

void SqlDatabase::sqlDeleteOffline(int userid)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString delete_sql = "delete from offline where oId = ?";
    query.prepare(delete_sql);
    query.addBindValue(userid);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}

int SqlDatabase::sqlSelectFaile(int id)
{
    m_mutex.lock();
    int value = 0;
    QSqlQuery query2(m_database);
    QString query_sql = "select type from insertFail where id = ?";
    query2.prepare(query_sql);
    query2.addBindValue(id);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    } else {
        while(query2.next()) {
            value = query2.value(0).toInt();
        }
    }
    m_mutex.unlock();
    return value;
}

void SqlDatabase::sqlInsertFail(int id, int type)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString insert_sql = "insert into insertFail values (?, ?)";
    query2.prepare(insert_sql);
    query2.addBindValue(id);
    query2.addBindValue(type);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_localFaceFail.insert(id);
    m_mutex.unlock();
}

void SqlDatabase::sqlDeleteFail(int id)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from insertFail where id = ?";
    query2.prepare(delete_sql);
    query2.addBindValue(id);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_localFaceFail.remove(id);
    m_mutex.unlock();
}

void SqlDatabase::sqlDeleteAllFail()
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from insertFail";
    query2.prepare(delete_sql);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_localFaceFail.clear();
    m_mutex.unlock();
}

void SqlDatabase::sqlInsertAuth(int id, int passNum, int isBlack, const QStringList &datas)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString insert_sql = "insert into auth values (?, ?, ?, ?, ?, ?, ?, ?)";
    query2.prepare(insert_sql);
    query2.addBindValue(id);
    query2.addBindValue(passNum);
    query2.addBindValue(datas.at(0));
    query2.addBindValue(datas.at(1));
    query2.addBindValue(isBlack);
    query2.addBindValue(datas.at(2));
    query2.addBindValue(datas.at(3));
    query2.addBindValue(datas.at(4));
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlUpdateAuth(int id, int passNum, int isBlack, const QStringList &datas)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString update_sql = "update auth set passNum = :passNum, startTime = :startTime, expireTime = :expireTime, isBlack = :isBlack, "
                         "passPeriod = :passPeriod, passTimeSection = :passTimeSection, remark = :remark where id = :id";
    query.prepare(update_sql);
    query.bindValue(":passNum", passNum);
    query.bindValue(":startTime", datas.at(0));
    query.bindValue(":expireTime", datas.at(1));
    query.bindValue(":isBlack", isBlack);
    query.bindValue(":passPeriod", datas.at(2));
    query.bindValue(":passTimeSection", datas.at(3));
    query.bindValue(":remark", datas.at(4));
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlDeleteAuth(int id)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from auth where id = ?";
    query2.prepare(delete_sql);
    query2.addBindValue(id);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlDeleteAllAuth()
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from auth";
    query2.prepare(delete_sql);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlUpdatePassNum(int id, int passNum)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString update_sql = "update auth set passNum = :passNum where id = :id";
    query.prepare(update_sql);
    query.bindValue(":passNum", passNum);
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}

QVariantList SqlDatabase::sqlSelectAuth(int id)
{
    m_mutex.lock();
    QVariantList values;
    QSqlQuery query(m_database);
    QString query_sql = "select * from auth where id = ?";
    query.prepare(query_sql);
    query.addBindValue(id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            values << query.value(0) << query.value(1) << query.value(2) << query.value(3) << query.value(4)
                   << query.value(5) << query.value(6) << query.value(7);
        }
    }
    m_mutex.unlock();
    return values;
}


void SqlDatabase::sqlInsertIc(int id, const QString &cardNo)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString insert_sql = "insert into iccard values (?, ?)";
    query2.prepare(insert_sql);
    query2.addBindValue(id);
    query2.addBindValue(cardNo);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlUpdateIc(int id, const QString &cardNo)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString update_sql = "update iccard set cardNo = :cardNo where id = :id";
    query.prepare(update_sql);
    query.bindValue(":cardNo", cardNo);
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlDeleteIc(int id)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from auth where id = ?";
    query2.prepare(delete_sql);
    query2.addBindValue(id);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlDeleteAllIc()
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from auth";
    query2.prepare(delete_sql);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

QString SqlDatabase::sqlSelectIc(int id)
{
    m_mutex.lock();
    QString value = "";
    QSqlQuery query(m_database);
    QString query_sql = "select cardNo from auth where id = ?";
    query.prepare(query_sql);
    query.addBindValue(id);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            value = query.value(0).toString();
        }
    }
    m_mutex.unlock();
    return value;
}

int SqlDatabase::sqlSelectIcId(const QString &cardNo)
{
    m_mutex.lock();
    int value = 0;
    QSqlQuery query(m_database);
    QString query_sql = "select id from auth where cardNo = ?";
    query.prepare(query_sql);
    query.addBindValue(cardNo);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            value = query.value(0).toInt();
        }
    }
    m_mutex.unlock();
    return value;
}
