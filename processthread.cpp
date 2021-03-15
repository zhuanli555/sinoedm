#include "processthread.h"

ProcessThread::ProcessThread(QObject *parent) :
    QThread(parent)
{
}

ProcessThread::~ProcessThread()
{
    m_mutex.lock();
    m_quit = true;
    m_mutex.unlock();
    wait();
}

void ProcessThread::run()
{

    while (!m_quit) {

        m_mutex.lock();

        m_mutex.unlock();
        QThread::msleep(m_waitTimeout);
    }
}
