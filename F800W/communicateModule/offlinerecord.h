#ifndef OFFLINERECORD_H
#define OFFLINERECORD_H
#include <QThread>
#include "sqldatabase.h"
#include "switch.h"

class OfflineRecord : public QThread
{
    Q_OBJECT
public:
    explicit OfflineRecord();

protected:
    void run();

signals:
    // 实时上传记录
    void uploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, const QStringList &datas);

private:

};
#endif // OFFLINERECORD_H
