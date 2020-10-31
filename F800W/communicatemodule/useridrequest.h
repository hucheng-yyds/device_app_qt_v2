#ifndef USERIDREQUEST_H
#define USERIDREQUEST_H
#include <QThread>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "sqldatabase.h"

class UserIdRequest : public QThread
{
    Q_OBJECT
public:
    explicit UserIdRequest();

protected:
    virtual void run();

public slots:
    void onAlluserId(QJsonArray &jsonArr);

private:
    QMutex m_mutex;
};

#endif // USERIDREQUEST_H
