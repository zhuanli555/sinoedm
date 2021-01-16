#ifndef ALARMSIGNAL_H
#define ALARMSIGNAL_H

#include <QWidget>
#include "EDM/EDM.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <EDM/EDM.h>

class AlarmSignal : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmSignal(QWidget *parent = nullptr);
    int addAlarm(const QString &text);
    void removeAlarm(int index);

private:
    QGridLayout* mainLayout;
    EDM *edm;
    QLabel* purgeValue;
    QLabel* shakeValue;
    QLabel* protectValue;
    QLabel* speedValue;
    QLabel* highFreqValue;
    QLabel* rotateValue;
    QLabel* fixElecValue;
    QList<QLabel*> alarmList;
private:
    void reSort();
signals:

public slots:
    void edmProtect();
    void edmPurge();
    void edmShake();
    void edmHighFreq();
};

#endif // ALARMSIGNAL_H
