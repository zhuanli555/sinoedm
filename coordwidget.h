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
    void ShowMacUserStatus();
    QString float2QString(float value);
private:
    static CoordWidget* m_coordWid;
    QVector<QLabel*> m_labels;

signals:
public slots:
    void HandleEdmCycleData();
    void setLabels();
};

#endif // COORDWIDGET_H
