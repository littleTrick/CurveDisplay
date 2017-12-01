#ifndef LOGGING_H
#define LOGGING_H
#include <QObject>

class Logging:public QObject
{
public:
    Logging();
    void outputMessage(QtMsgType type, const QString &msg);
};

#endif // LOGGING_H
