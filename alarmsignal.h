#ifndef ALARMSIGNAL_H
#define ALARMSIGNAL_H

#include <QWidget>
#include "EDM/EDM.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>

class AlarmSignal : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmSignal(QWidget *parent = nullptr);
    int addAlarm(const QString &text);
    void removeAlarm(int index);

private:
    QGridLayout* mainLayout;
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
};

#endif // ALARMSIGNAL_H
