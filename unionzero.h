#ifndef UNIONZERO_H
#define UNIONZERO_H

#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QDialog>
#include <QCheckBox>
#include <QButtonGroup>
#include "EDM/EDM.h"
#include "EDM/cmdhandle.h"

class UnionZero : public QDialog
{
    Q_OBJECT
public:
    explicit UnionZero(QWidget *parent = nullptr);
    explicit UnionZero(int key, QWidget *parent = 0);
    ~UnionZero();

public:
    EDM* edm;
private:
    QGridLayout* mainLayout;
    QLabel* w;
    QLabel* esc;
    QCheckBox* wx;
    QCheckBox* wy;
    QCheckBox* ww;
    QCheckBox* wa;
    QCheckBox* wb;
    QCheckBox* wc;
    QCheckBox* wz;
    QCheckBox* wall;
    QButtonGroup* group;
    QLineEdit* lineEdit;
    QLabel* label2;
    int workflag;//工作零标志
    bool bZero[MAC_LABEL_COUNT] = {false};
private:
    void axisWorkZero(int label);
signals:
    void rtZeroSig(int label);
    void setAxisSig(int label,QString str);
    void edmMoveParaSendSig(DIGIT_CMD*);
protected slots:

    void chooseAll();
    void buttonGroupClicked();
    void accept() override;

};
#endif // UNIONZERO_H
