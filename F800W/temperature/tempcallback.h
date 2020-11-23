#ifndef TEMPCALLBACK_H
#define TEMPCALLBACK_H
#include <QObject>

class TempCallBack
{
public:
    ~TempCallBack();
    ///播放视频，此函数不宜做耗时操作，否则会影响播放的流畅性。
    virtual void setTempResult(const QString &tempVal, int result) = 0;

};


#endif // TEMPCALLBACK_H
