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
    void ShowAxisData();
    void ShowData(const MAC_COMMON& stMaccomm,int iRelLabel[]);
    void ShowMacUserStatus();
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
