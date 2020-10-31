#include "useridrequest.h"

UserIdRequest::UserIdRequest()
{

}

void UserIdRequest::run()
{

}

void UserIdRequest::onAlluserId(QJsonArray &jsonArr)
{
    m_mutex.lock();
    QJsonArray faceJson = jsonArr;
    int count = sqlDatabase->m_localFaceSet.size();
    qDebug() << "OnAllUserId count:" << faceJson.count() << count;
    if(faceJson.count() > 0)
    {
//        QMap<>
    }
}
