#include "axissetdialog.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QtGui/QKeyEvent>

AxisSetDialog::AxisSetDialog(int key, QWidget *parent)
    : QDialog(parent)
{
    QString str;
    edm = EDM::GetEdmInstance();
    switch (key) {
    case Qt::Key_X:
        str = QString::fromLocal8Bit("X轴设置值");
        m_str = 'X';
        break;
    case Qt::Key_Y:
        str = QString::fromLocal8Bit("Y轴设置值");
        m_str = 'Y';
        break;
    case Qt::Key_W:
        str = QString::fromLocal8Bit("W轴设置值");
        m_str = 'W';
        break;
    case Qt::Key_Z:
        str = QString::fromLocal8Bit("Z轴设置值");
        m_str = 'Z';
        break;
    case Qt::Key_A:
        str = QString::fromLocal8Bit("A轴设置值");
        m_str = 'A';
        break;
    case Qt::Key_B:
        str = QString::fromLocal8Bit("B轴设置值");
        m_str = 'B';
        break;
    default:
        m_str = '\0';
        break;
    }
    label2 =new QLabel(str);
    lineEdit2 =new QLineEdit;
    connect(lineEdit2,&QLineEdit::returnPressed,this,&AxisSetDialog::returnValue);
    QGridLayout *TopLayout =new QGridLayout(this);
    TopLayout->addWidget(label2,0,0);
    TopLayout->addWidget(lineEdit2,0,1);
}

AxisSetDialog::AxisSetDialog(QWidget *parent)
    : QDialog(parent)
{
    label2 =new QLabel(QString::fromLocal8Bit("请选择合适的轴"),this);
    QGridLayout *TopLayout =new QGridLayout(this);
    TopLayout->addWidget(label2,0,0);
}


AxisSetDialog::~AxisSetDialog()
{

}

void AxisSetDialog::returnValue()
{
    m_strVal = lineEdit2->text();
    accept();
}

void AxisSetDialog::axisSet()
{
    CmdHandle *pCmdHandle;
    static DIGIT_CMD stDigitCmd;
    DIGIT_CMD cmdDefault;
    QString cmd = "G92 G00";
    cmd += m_str;
    cmd += m_strVal;
    cmdDefault.enCoor = edm->m_stEdmShowData.enCoorType;
    pCmdHandle = new CmdHandle(FALSE,cmd,&stDigitCmd,&cmdDefault);
    delete pCmdHandle;
    //多线程？
    edm->EdmSendMovePara(&stDigitCmd);
}

void AxisSetDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
}
