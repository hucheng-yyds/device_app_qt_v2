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
        m_database.setDatabaseName("faceDatas.db");
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
                    "cardNo text,"
                    "passNum int,"
                    "startTime text,"
                    "expireTime text,"
                    "isBlack int,"
                    "passPeriod text,"
                    "passTimeSection text,"
                    "mobile text,"
                    "photoName text,"
                    "remark text)")) {
        qDebug() << query.lastError();
    }
    QSqlQuery query1(m_database);
    if (!query1.exec("create table offline ("
                    "id int primary key,"
                    "userId int,"
                    "unlockType int,"
                    "unlockTime text,"
                    "temperature text,"
                    "isStranger text,"
                    "isSuccess text,"
                    "reason text,"
                    "cardNo text)")) {
        qDebug() << query1.lastError();
    }
    QSqlQuery query2(m_database);
    if (!query2.exec("create table insertFail ("
                    "id int primary key,"
                    "username text,"
                    "edittime text,"
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
}


QString SqlDatabase::sqlSelectPhotoName(int id)
{
    m_mutex.lock();
    QString value = "";
    QSqlQuery query(m_database);
    QString query_sql = "select photoName from userdata where id = ?";
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
            value << query.value(0) << query.value(1) << query.value(2) << query.value(3) << query.value(4) << query.value(5)
                  << query.value(6) << query.value(7) << query.value(8) << query.value(9) << query.value(10) << query.value(11) << query.value(12) << query.value(13);
        }
    }
    m_mutex.unlock();
    return value;
}

void SqlDatabase::sqlSelectAllUserId()
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString query_sql = "select id from userdata";
    query.prepare(query_sql);
    if (!query.exec()) {
        qDebug() << query.lastError();
    } else {
        while(query.next()) {
            m_localFaceSet.insert(query.value(0).toInt());
        }
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlInsert(int id, int passnum, int isBack, const QVariant &feature, const QStringList &data)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString insert_sql = "insert into userdata values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    query.prepare(insert_sql);
    query.addBindValue(id);
    query.addBindValue(data.at(0));
    query.addBindValue(data.at(1));
    query.addBindValue(feature);
    query.addBindValue(data.at(2));
    query.addBindValue(passnum);
    query.addBindValue(data.at(3));
    query.addBindValue(data.at(4));
    query.addBindValue(isBack);
    query.addBindValue(data.at(5));
    query.addBindValue(data.at(6));
    query.addBindValue(data.at(7));
    query.addBindValue(data.at(8));
    query.addBindValue(data.at(9));
    if (!query.exec()) {
        qDebug() << query.lastError() << data;
    }
    m_localFaceSet.insert(id);
    m_mutex.unlock();
}

void SqlDatabase::sqlUpdatePass(int id, int passnum)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString update_sql = "update userdata set passNum = :passNum where id = :id";
    query.prepare(update_sql);
    query.bindValue(":passNum", passnum);
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}
void SqlDatabase::sqlUpdate(int id, int passnum, int isBack, const QStringList &data)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString update_sql = "update userdata set username = :username, edittime = :edittime, cardNo = :cardNo, passNum = :passNum, startTime = :startTime, expireTime = :expireTime"
                         ", isBlack = :isBlack, passPeriod = :passPeriod, passTimeSection = :passTimeSection, mobile = :mobile"
                         ", photoName = :photoName, remark = :remark where id = :id";
    query.prepare(update_sql);
    query.bindValue(":username", data.at(0));
    query.bindValue(":edittime", data.at(1));
    query.bindValue(":cardNo", data.at(2));
    query.bindValue(":passNum", passnum);
    query.bindValue(":startTime", data.at(2));
    query.bindValue(":expireTime", data.at(3));
    query.bindValue(":isBlack", isBack);
    query.bindValue(":passPeriod", data.at(4));
    query.bindValue(":passTimeSection", data.at(5));
    query.bindValue(":mobile", data.at(6));
    query.bindValue(":photoName", data.at(7));
    query.bindValue(":remark", data.at(8));
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}

QString SqlDatabase::sqlGetFeature(int id)
{
    m_mutex.lock();
    QString value;
    QSqlQuery query(m_database);
    QString query_sql = "select feature from userdata where id = ?";
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

void SqlDatabase::sqlDeleteAll()
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString delete_sql = "delete from userdata";
    query.prepare(delete_sql);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    qDebug() << "clear all face";
    m_localFaceSet.clear();
    m_mutex.unlock();
}

void SqlDatabase::sqlDelete(int id)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString delete_sql = "delete from userdata where id = ?";
    query.prepare(delete_sql);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    m_localFaceSet.remove(id);
    m_mutex.unlock();
}
