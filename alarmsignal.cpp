#include "alarmsignal.h"
#include <QTextCodec>

AlarmSignal::AlarmSignal(QWidget *parent) : QWidget(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    setMaximumWidth(600);
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
