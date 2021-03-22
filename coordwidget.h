#ifndef COORDWIDGET_H
#define COORDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QGridLayout>
#include "EDM/EDM.h"



class CoordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CoordWidget(QWidget *parent = nullptr);
    ~CoordWidget();
    void HandleEdmCycleData();
    static CoordWidget* getInstance();
    void setAxisValue(int label,QString str);

public :
    EDM* edm;
	unsigned char m_bRunThread;
	unsigned char m_bThFirstOver;
	unsigned char m_bThSecOver;
	unsigned char m_bShowBig;
	unsigned char m_bLuoBuTest;
    unsigned char m_bPause;
	EDM_COOR_TYPE m_enCoorType;

private:
    void setLabels();
    void ShowAxisData();
    void ShowData(const MAC_COMMON& stMaccomm,int iRelLabel[]);
    void ShowMacUserStatus();
    void SaveData();
    QString float2QString(float value);
private:
    static CoordWidget* m_coordWid;
    QLabel* labels[MAC_LABEL_COUNT] = {xLabel,yLabel,cLabel,wLabel,aLabel,bLabel,zLabel};
    QLabel* shows[MAC_LABEL_COUNT] = {xShow,yShow,cShow,wShow,aShow,bShow,zShow};
    QLabel* mValues[MAC_LABEL_COUNT] = {xMValue,yMValue,cMValue,wMValue,aMValue,bMValue,zMValue};
    QLabel* Values[MAC_LABEL_COUNT] = {xValue,yValue,cValue,wValue,aValue,bValue,zValue};
    //left
    QGridLayout *leftLayout;
    QImage img;
    QLabel* xLabel;
    QLabel* yLabel;
    QLabel* cLabel;
    QLabel* wLabel;
    QLabel* aLabel;
    QLabel* bLabel;
    QLabel* zLabel;
    QLabel* xValue;
    QLabel* yValue;
    QLabel* cValue;
    QLabel* wValue;
    QLabel* aValue;
    QLabel* bValue;
    QLabel* zValue;
    QLabel* xMValue;
    QLabel* yMValue;
    QLabel* cMValue;
    QLabel* wMValue;
    QLabel* aMValue;
    QLabel* bMValue;
    QLabel* zMValue;
    QLabel* xShow;
    QLabel* yShow;
    QLabel* cShow;
    QLabel* wShow;
    QLabel* aShow;
    QLabel* bShow;
    QLabel* zShow;

protected:
signals:

public slots:
};

#endif // COORDWIDGET_H
