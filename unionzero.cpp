#include "unionzero.h"
#include <QtGui/QKeyEvent>
#include <QTextCodec>
#include <QDialogButtonBox>

UnionZero::UnionZero(QWidget *parent) : QDialog(parent)
{

}

UnionZero::UnionZero(int key, QWidget *parent) : QDialog(parent)
{
    setGeometry(400, 100, 300, 400);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    setWindowTitle(QString::fromLocal8Bit("轴选择"));
    setStyleSheet("QLabel{\
                    border:3px solid rgb(60,60,60);margin:2px 2px;}");
    edm = EDM::GetEdmInstance();
    workflag = key;
    group = new QButtonGroup(this);
    wx = new QCheckBox(QString::fromLocal8Bit("选择X轴\n(x)"));
    wy = new QCheckBox(QString::fromLocal8Bit("选择Y轴\n(y)"));
    wc = new QCheckBox(QString::fromLocal8Bit("选择C轴\n(c)"));
    ww = new QCheckBox(QString::fromLocal8Bit("选择W轴\n(w)"));
    wa = new QCheckBox(QString::fromLocal8Bit("选择A轴\n(a)"));
    wb = new QCheckBox(QString::fromLocal8Bit("选择B轴\n(b)"));
    wz = new QCheckBox(QString::fromLocal8Bit("选择Z轴\n(z)"));
    wall = new QCheckBox(QString::fromLocal8Bit("选择全部"));
    group->addButton(wx, 0);
    group->addButton(wy, 1);
    group->addButton(wc, 2);
    group->addButton(ww, 3);
    group->addButton(wa, 4);
    group->addButton(wb, 5);
    group->addButton(wz, 6);
    connect(wx, &QCheckBox::clicked, this, &UnionZero::buttonGroupClicked);
    connect(wy, &QCheckBox::clicked, this, &UnionZero::buttonGroupClicked);
    connect(wc, &QCheckBox::clicked, this, &UnionZero::buttonGroupClicked);
    connect(ww, &QCheckBox::clicked, this, &UnionZero::buttonGroupClicked);
    connect(wa, &QCheckBox::clicked, this, &UnionZero::buttonGroupClicked);
    connect(wb, &QCheckBox::clicked, this, &UnionZero::buttonGroupClicked);
    connect(wz, &QCheckBox::clicked, this, &UnionZero::buttonGroupClicked);
    connect(wall, &QCheckBox::clicked, this, &UnionZero::chooseAll);
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(wx, 0, 0);
    mainLayout->addWidget(wy, 0, 1);
    mainLayout->addWidget(ww, 0, 2);
    mainLayout->addWidget(wc, 1, 0);
    mainLayout->addWidget(wa, 1, 1);
    mainLayout->addWidget(wb, 1, 2);
    mainLayout->addWidget(wz, 2, 0);
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line, 2, 0, 1, 3);

    QDialogButtonBox *button = new QDialogButtonBox(this);
    button->addButton(QString::fromLocal8Bit("确认"), QDialogButtonBox::YesRole);
    button->addButton(QString::fromLocal8Bit("取消"), QDialogButtonBox::NoRole);
    connect(button, SIGNAL(accepted()), this, SLOT(accept()));
    connect(button, SIGNAL(rejected()), this, SLOT(reject()));
    if (workflag == 2) //轴置数
    {
        label2 = new QLabel(QString::fromLocal8Bit("输入值:"));
        lineEdit = new QLineEdit;
        mainLayout->addWidget(label2, 3, 0);
        mainLayout->addWidget(lineEdit, 3, 1);
        mainLayout->addWidget(wall, 3, 2);
        mainLayout->addWidget(button, 4, 0, 1, 3);
    }
    else
    {
        mainLayout->addWidget(button, 3, 0, 1, 3);
    }
    connect(this,&UnionZero::edmMoveParaSendSig,edm,&EDM::EdmSendMovePara);
}

UnionZero::~UnionZero()
{

}

void UnionZero::axisWorkZero(int label)
{
    DIGIT_CMD stDigitCmd;
    memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
    stDigitCmd.enAim = AIM_G90;
    stDigitCmd.enOrbit = ORBIT_G00;
    stDigitCmd.enCoor = edm->m_stEdmShowData.enCoorType;
    stDigitCmd.stOp.bShortDis = TRUE;
    stDigitCmd.iAxisCnt++;
    stDigitCmd.stAxisDigit[0].iDistance = 0;
    stDigitCmd.stAxisDigit[0].iLabel = label;
    emit edmMoveParaSendSig(&stDigitCmd);
}

void UnionZero::chooseAll()
{
    for(int i= 0;i<MAC_LABEL_COUNT;i++)bZero[i] = true;
}

void UnionZero::buttonGroupClicked()
{
    int index = -1;
    if(wx->isChecked())index = 0;
    if(wy->isChecked())index = 1;
    if(wc->isChecked())index = 2;
    if(ww->isChecked())index = 3;
    if(wa->isChecked())index = 4;
    if(wb->isChecked())index = 5;
    if(wz->isChecked())index = 6;
    bZero[index] = true;
}

void UnionZero::accept()
{
    int i = 0;
    QChar chas[MAC_LABEL_COUNT]={'X','Y','C','W','A','B','Z'};
    if (workflag == 0){
        //回机械零
        for(i = 0;i < MAC_LABEL_COUNT;i++)
        {
            if(bZero[i])
            {
                if(edm->EdmRtZero(i))
                emit rtZeroSig(i);
            }
        }
    }
    else if(workflag == 1)
    {
        //回工作零
        for(i = 0;i < MAC_LABEL_COUNT;i++)
        {
            if(bZero[i])
            {
                axisWorkZero(i);
            }
        }
    }else
    {
        //轴置数
        CmdHandle *pCmdHandle;
        static DIGIT_CMD stDigitCmd;

        QString sVal = "0";
        DIGIT_CMD cmdDefault;
        memset(&cmdDefault,0,sizeof(DIGIT_CMD));
        cmdDefault.enCoor = edm->m_stEdmShowData.enCoorType;
        QString cmd = "G92 G00 ";
        if(!lineEdit->text().isEmpty())sVal = lineEdit->text();
        for(i = 0;i<MAC_LABEL_COUNT;i++)
        {
            if(bZero[i])
            {
                cmd += chas[i];
                cmd += sVal;
                cmd += " ";
                emit setAxisSig(i,sVal);
            }

        }

        pCmdHandle = new CmdHandle(FALSE,cmd,&stDigitCmd,&cmdDefault);
        delete pCmdHandle;
        emit edmMoveParaSendSig(&stDigitCmd);
    }
    return QDialog::accept();
}

