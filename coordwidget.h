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
    static CoordWidget* m_coordWid;
    //left
    QGridLayout *leftLayout;
    QImage img;
    QLabel* xLabel;
    QLabel* yLabel;
    QLabel* cLabel;
    QLabel* wLabel;
    QLabel* aLabel;
    QLabel* bLabel;
    QLabel* xValue;
    QLabel* yValue;
    QLabel* cValue;
    QLabel* wValue;
    QLabel* aValue;
    QLabel* bValue;
    QLabel* xMValue;
    QLabel* yMValue;
    QLabel* cMValue;
    QLabel* wMValue;
    QLabel* aMValue;
    QLabel* bMValue;

    QLabel* xShow;
    QLabel* yShow;
    QLabel* cShow;
    QLabel* wShow;
    QLabel* aShow;
    QLabel* bShow;

protected:
signals:

public slots:
};

#endif // COORDWIDGET_H
