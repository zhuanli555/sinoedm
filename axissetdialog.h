#ifndef AXISSETDIALOG_H
#define AXISSETDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include "EDM/cmdhandle.h"
#include "EDM/EDM.h"

class AxisSetDialog : public QDialog
{
public:
    explicit AxisSetDialog(int key, QWidget *parent = 0);
    explicit AxisSetDialog(QWidget *parent = 0);
    ~AxisSetDialog();
    void createContentFrame();		//完成窗体的创建
    void axisSet();
public:
    QChar m_str;
    QString m_strVal;

private slots:
    void returnValue();
protected:
    void keyPressEvent(QKeyEvent *e) override;
private:
    QFrame *contentFrame;              	//具体显示面板
    QLabel *label2;
    QLineEdit *lineEdit2;
    EDM *edm;
};

#endif // AXISSETDIALOG_H
