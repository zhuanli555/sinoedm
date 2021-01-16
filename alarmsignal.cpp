#include "alarmsignal.h"
#include <QTextCodec>

AlarmSignal::AlarmSignal(QWidget *parent) : QWidget(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    setMaximumWidth(600);
    edm = EDM::GetEdmInstance();
    purgeValue = new QLabel(QString::fromLocal8Bit("冲液(F4)"));
    fixElecValue = new QLabel(QString::fromLocal8Bit("修电极(F5)"));
    highFreqValue = new QLabel(QString::fromLocal8Bit("高频(F6)"));
    shakeValue = new QLabel(QString::fromLocal8Bit("振动(F7)"));
    protectValue = new QLabel(QString::fromLocal8Bit("防撞保护(F8)"));
    rotateValue = new QLabel(QString::fromLocal8Bit("旋转(F9)"));

    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->addWidget(purgeValue,0,0);
    mainLayout->addWidget(fixElecValue,1,0);
    mainLayout->addWidget(highFreqValue,2,0);
    mainLayout->addWidget(shakeValue,3,0);
    mainLayout->addWidget(protectValue,4,0);
    mainLayout->addWidget(rotateValue,5,0);

    protectValue->setMaximumWidth(85);
    purgeValue->setStyleSheet("background-color:red;");
    shakeValue->setStyleSheet("background-color:red;");
    protectValue->setStyleSheet("background-color:green;");
    highFreqValue->setStyleSheet("background-color:green;");
    fixElecValue->setStyleSheet("background-color:red;");
    rotateValue->setStyleSheet("background-color:red;");
}

int AlarmSignal::addAlarm(const QString &text)
{

    QLabel* pLabel = new QLabel(text);
    pLabel->setStyleSheet("background-color:red;");
    pLabel->setMaximumWidth(85);
    alarmList.append(pLabel);
    reSort();
    return alarmList.size()-1;
}

void AlarmSignal::removeAlarm(int index)
{
    alarmList.removeAt(index);
    reSort();
}

void AlarmSignal::reSort()
{
    int i ,j;
    if(alarmList.isEmpty())return;
    for(i = 0; i < alarmList.size();i++)
    {
        j = i/6+1;//column
        mainLayout->addWidget(alarmList.at(i),i%6,j);
    }
}

void AlarmSignal::edmPurge()
{

    edm->EdmLowPump(!edm->m_stEdmShowData.stStatus.bPumpLow);//低压 冲液？
    if(edm->m_stEdmShowData.stStatus.bPumpLow)
    {
        purgeValue->setStyleSheet("background-color:red;");
    }
    else{
        purgeValue->setStyleSheet("background-color:green;");
    }
}

void AlarmSignal::edmHighFreq()
{
    qDebug()<<"alarmhighfreq";
    edm->EdmPower(!edm->m_stEdmShowData.stStatus.bPower);
    if(edm->m_stEdmShowData.stStatus.bPower)
    {
        highFreqValue->setStyleSheet("background-color:red;");
    }
    else{
        highFreqValue->setStyleSheet("background-color:green;");
    }
}

void AlarmSignal::edmProtect()
{
    //多线程
    edm->EdmSetProtect(edm->m_stEdmShowData.stStatus.bNoProtect);
    if(edm->m_stEdmShowData.stStatus.bNoProtect)
    {
        protectValue->setStyleSheet("background-color:green;");
    }else{
        protectValue->setStyleSheet("background-color:red;");
    }
}



void AlarmSignal::edmShake()
{
    edm->EdmSetShake(edm->m_stEdmShowData.stStatus.bShake);
    if(edm->m_stEdmShowData.stStatus.bShake)
    {
        shakeValue->setStyleSheet("background-color:green;");
    }else{
        shakeValue->setStyleSheet("background-color:red;");
    }
}

