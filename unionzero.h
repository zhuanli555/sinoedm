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
    QCheckBox* wall;
    QButtonGroup* group;
    QLineEdit* lineEdit;
    QLabel* label2;
    int workflag;//工作零标志
    bool bZero[MAC_LABEL_COUNT] = {false};
private:
    void axisWorkZero(int label);

protected slots:

    void chooseAll();
    void buttonGroupClicked(QAbstractButton*);
    void accept() override;

};
#endif // UNIONZERO_H
