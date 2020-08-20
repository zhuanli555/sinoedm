#ifndef UNIONZERO_H
#define UNIONZERO_H

#include <QLabel>
#include <QGridLayout>
#include <QDialog>
#include "EDM/EDM.h"

class UnionZero : public QDialog
{
public:
    explicit UnionZero(QWidget *parent = nullptr);
    ~UnionZero();

public:
    EDM* edm;

private:
    QGridLayout* mainLayout;
    QLabel* w;
    QLabel* esc;
    QLabel* wx;
    QLabel* wy;
    QLabel* ww;
    QLabel* wa;
    QLabel* wb;
    QLabel* wc;
    QLabel* m;
    QLabel* mx;
    QLabel* my;
    QLabel* mw;
    QLabel* ma;
    QLabel* mb;
    QLabel* mc;

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // UNIONZERO_H
