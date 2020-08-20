#ifndef COORDWIDGET_H
#define COORDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QGridLayout>
#include "EDM/EDM.h"

typedef struct edm_Show_Status
{
    BOOL bStop;
    int iStopCnt;
    BOOL bRTzero;
    int iRTzeroLabel;
    BOOL bOpIn;
    BOOL bAlarmLimit;
}EDM_SHOW_STATUS;

class CoordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CoordWidget(QWidget *parent = nullptr);
    ~CoordWidget();
    void HandleEdmCycleData();

public :
    EDM* edm;
	BOOL m_bRunThread;
	BOOL m_bThFirstOver;
	BOOL m_bThSecOver;
	BOOL m_bShowBig;
	BOOL m_bLuoBuTest;
    BOOL m_bPause;
	EDM_COOR_TYPE m_enCoorType;
    EDM_SHOW_STATUS m_stEntileStatus;
private:
    void ShowAxisData();
    void ShowData(const MAC_COMMON& stMaccomm,int iRelLabel[]);
    void SaveData();
private:
    //left
    QGridLayout *leftLayout;
    QImage img;
    QLabel* xLabel;
    QLabel* yLabel;
    QLabel* zLabel;
    QLabel* xValue;
    QLabel* yValue;
    QLabel* zValue;
    QLabel* xMValue;
    QLabel* yMValue;
    QLabel* zMValue;
    QLabel* wLabel;
    QLabel* aLabel;
    QLabel* bLabel;
    QLabel* wValue;
    QLabel* aValue;
    QLabel* bValue;
    QLabel* wMValue;
    QLabel* aMValue;
    QLabel* bMValue;

protected:
signals:

public slots:
};

#endif // COORDWIDGET_H
