#ifndef ALARMSIGNAL_H
#define ALARMSIGNAL_H

#include <QWidget>
#include "EDM/EDM.h"
#include "EDM/EDM_OP_List.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <EDM/EDM.h>

typedef struct edm_Show_Status
{
    unsigned char bStop;
    int iStopCnt;
    unsigned char bRTzero;
    int iRTzeroLabel;
    unsigned char bOpIn;
    unsigned char bAlarmLimit;
}EDM_SHOW_STATUS;

class AlarmSignal : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmSignal(QWidget *parent = nullptr);
    int addAlarm(const QString &text);
    void removeAlarm(int index);
    void reSort();
    void EdmStatusSignChange();
    static AlarmSignal* getInstance();
    void edmHandProcess();
public:
    EDM_SHOW_STATUS m_stEntileStatus;
    bool bPause;
private:
    QGridLayout* mainLayout;
    EDM *edm;
    static AlarmSignal* m_alarmSig;
    EDM_OP_List* edmOpList;
    QLabel* purgeValue;
    QLabel* shakeValue;
    QLabel* protectValue;
    QLabel* speedValue;
    QLabel* lowPumpValue;
    QLabel* fixElecValue;
    QLabel* findCenter;
    QLabel* pauseValue;
    QList<QLabel*> alarmList;

private:

signals:

public slots:
    void edmProtect();
    void edmPurge();
    void edmShake();
    void edmLowerPump();
    void edmPause();
    void edmStop();
};

#endif // ALARMSIGNAL_H
