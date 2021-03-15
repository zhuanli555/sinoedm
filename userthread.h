#ifndef USERTHREAD_H
#define USERTHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class UserThread : public QThread
{
    Q_OBJECT

public:
    explicit UserThread(QObject *parent = nullptr);
    ~UserThread();

signals:
    void request(const QString &s);

private:
    void run() override;
    int m_waitTimeout = 35;
    QMutex m_mutex;
    bool m_quit = false;
};

#endif // USERTHREAD_H
