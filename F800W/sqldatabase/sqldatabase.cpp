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
                    "feature text)")) {
        qDebug() << query.lastError();
    }

    QSqlQuery query1(m_database);
    if (!query1.exec("create table offline ("
                    "userId int primary key,"
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
            value << query.value(0) << query.value(1) << query.value(2) << query.value(3);
        }
    }
    m_mutex.unlock();
    return value;
}

QMap<int, QString> SqlDatabase::sqlSelectAllUserIdTime()
{
    m_mutex.lock();
    QMap<int, QString> useridTime;
    useridTime.clear();
    QSqlQuery query(m_database);
    QString query_sql = "select id, edittime from userdata";
    query.prepare(query_sql);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        while(query.next())
        {
            useridTime.insert(query.value(0).toInt(), query.value(1).toString());
        }
    }
    m_mutex.unlock();
    return useridTime;
}

QMap<int, QString> SqlDatabase::sqlSelectAllUserIdFeature()
{
    m_mutex.lock();
    QMap<int, QString> useridFeature;
    useridFeature.clear();
    QSqlQuery query(m_database);
    QString query_sql = "select id, feature from userdata";
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
            useridFeature.insert(query.value(0).toInt(), query.value(1).toString());
        }
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlInsert(int id, const QString &username, const QString &time, const QString &feature)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString insert_sql = "insert into userdata values (?, ?, ?, ?)";
    query.prepare(insert_sql);
    query.addBindValue(id);
    query.addBindValue(username);
    query.addBindValue(time);
    query.addBindValue(feature);
    if (!query.exec())
    {
        qDebug() << query.lastError() << id;
    }
    m_localFaceSet.insert(id);
    m_mutex.unlock();
}


void SqlDatabase::sqlUpdate(int id, const QString &username, const QString &time)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString update_sql = "update userdata set username = :username, edittime = :edittime where id = :id";
    query.prepare(update_sql);
    query.bindValue(":username", username);
    query.bindValue(":edittime", time);
    query.bindValue(":id", id);
    if (!query.exec())
    {
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
    m_localFaceSet.remove(id);
    m_mutex.unlock();
}

void SqlDatabase::sqlInsertOffline(int userid, int type, int isOver, int isTemp, const QStringList &datas)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString insert_sql = "insert into offline values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    query.prepare(insert_sql);
    query.addBindValue(userid);
    query.addBindValue(type);
    query.addBindValue(datas.at(0));
    query.addBindValue(datas.at(1));
    query.addBindValue(isOver);
    query.addBindValue(isTemp);
    query.addBindValue(datas.at(2));
    query.addBindValue(datas.at(3));
    query.addBindValue(datas.at(4));
    query.addBindValue(datas.at(5));
    if (!query.exec())
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
                  << query.value(5) << query.value(6) << query.value(7) << query.value(8) << query.value(9) << query.value(10);
        }
    }
    m_mutex.unlock();
    return values;
}

void SqlDatabase::sqlDeleteOffline(int userid)
{
    m_mutex.lock();
    QSqlQuery query(m_database);
    QString delete_sql = "delete from offline where userId = ?";
    query.prepare(delete_sql);
    query.addBindValue(userid);
    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    m_mutex.unlock();
}

QSet<int> SqlDatabase::sqlInsertFailSelectAll()
{
    m_mutex.lock();
    QSet<int> value;
    QSqlQuery query2(m_database);
    QString query_sql = "select id from insertFail";
    query2.prepare(query_sql);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    } else {
        while(query2.next()) {
            value << query2.value(0).toInt();
        }
    }
    m_mutex.unlock();
    return value;
}

QVariantList SqlDatabase::sqlInsertFaileSelect(const QVariant &variant)
{
    m_mutex.lock();
    QVariantList value;
    value.clear();
    QSqlQuery query2(m_database);
    QString query_sql = "select id,edittime,username,type from insertFail where id = ?";
    query2.prepare(query_sql);
    query2.addBindValue(variant);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    } else {
        while(query2.next()) {
            value << query2.value(0) << query2.value(1) << query2.value(2) << query2.value(3);
        }
    }
    m_mutex.unlock();
    return value;
}

void SqlDatabase::sqlInsertFailInsert(const QVariant &id, const QVariant &name, const QVariant &edittime, const QVariant &feature)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString insert_sql = "insert into insertFail values (?, ?, ?, ?)";
    query2.prepare(insert_sql);
    query2.addBindValue(id);
    query2.addBindValue(name);
    query2.addBindValue(edittime);
    query2.addBindValue(feature);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlInsertFailDelete(const QVariant &id)
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from insertFail where id = ?";
    query2.prepare(delete_sql);
    query2.addBindValue(id);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}

void SqlDatabase::sqlInsertFailDeleteAll()
{
    m_mutex.lock();
    QSqlQuery query2(m_database);
    QString delete_sql = "delete from insertFail";
    query2.prepare(delete_sql);
    if (!query2.exec()) {
        qDebug() << query2.lastError();
    }
    m_mutex.unlock();
}
