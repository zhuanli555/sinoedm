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

private:
    void ShowAxisData();
    void ShowMacUserStatus();
    QString float2QString(float value);
private:
    QVector<QLabel*> m_labels;
    EDM* edm;

signals:
public slots:
    void HandleEdmCycleData();
    void setLabels();
};

#endif // COORDWIDGET_H
