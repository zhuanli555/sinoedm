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
    void createContentFrame();		//��ɴ���Ĵ���
    void axisSet();
public:
    QChar m_str;
    QString m_strVal;

private slots:
    void returnValue();
protected:
    void keyPressEvent(QKeyEvent *e) override;
private:
    QFrame *contentFrame;              	//������ʾ���
    QLabel *label2;
    QLineEdit *lineEdit2;
    EDM *edm;
};

#endif // AXISSETDIALOG_H
