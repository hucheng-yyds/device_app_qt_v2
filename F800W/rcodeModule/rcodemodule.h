#ifndef RCODEMODULE_H
#define RCODEMODULE_H
#include <QThread>

class RcodeModule : public QThread
{
    Q_OBJECT
public:
    explicit RcodeModule();

protected:
    virtual void run();

private:

};
#endif // RCODEMODULE_H
