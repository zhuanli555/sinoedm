#include "userthread.h"
#include "alarmsignal.h"
#include "coordwidget.h"

UserThread::UserThread(QObject *parent) :
    QThread(parent)
{
}

UserThread::~UserThread()
{
    m_mutex.lock();
    m_quit = true;
    m_mutex.unlock();
    wait();
}

void UserThread::run()
{
    AlarmSignal *al = AlarmSignal::getInstance();
    CoordWidget* cw = CoordWidget::getInstance();
    while (!m_quit) {

        m_mutex.lock();
        cw->HandleEdmCycleData();//�������������Դ���
        al->EdmStatusSignChange();//�����ź������Դ���
        m_mutex.unlock();
        QThread::msleep(m_waitTimeout);
    }
}
