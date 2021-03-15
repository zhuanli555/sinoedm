#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class ProcessThread : public QThread
{
    Q_OBJECT

public:
    explicit ProcessThread(QObject *parent = nullptr);
    ~ProcessThread();

signals:
    void request(const QString &s);

private:
    void run() override;
    int m_waitTimeout = 20;
    QMutex m_mutex;
    bool m_quit = false;
};

#endif // PROCESSTHREAD_H
