#include "unionzero.h"
#include <QtGui/QKeyEvent>
#include <QTextCodec>

UnionZero::UnionZero(QWidget *parent) : QDialog(parent)
{
    setGeometry(400,100,300,400);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    setWindowTitle(QString::fromLocal8Bit("�����λ"));
    setStyleSheet("QLabel{\
          border:3px solid rgb(60,60,60);margin:2px 2px;}");
    edm = EDM::GetEdmInstance();

    esc = new QLabel(QString::fromLocal8Bit("����\n(Esc)"));
    w = new QLabel(QString::fromLocal8Bit("ѡ����:"));
    wx =  new QLabel(QString::fromLocal8Bit("ѡ��X��\n(x)"));
    wy =  new QLabel(QString::fromLocal8Bit("ѡ��Y��\n(y)"));
    wc =  new QLabel(QString::fromLocal8Bit("ѡ��C��\n(c)"));
    ww =  new QLabel(QString::fromLocal8Bit("ѡ��W��\n(w)"));
    wa =  new QLabel(QString::fromLocal8Bit("ѡ��A��\n(a)"));
    wb =  new QLabel(QString::fromLocal8Bit("ѡ��B��\n(b)"));
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(esc,0,0,1,1);
    mainLayout->addWidget(w,0,1,1,2);
    mainLayout->addWidget(wx,1,0);
    mainLayout->addWidget(wy,1,1);
    mainLayout->addWidget(ww,1,2);
    mainLayout->addWidget(wc,2,0);
    mainLayout->addWidget(wa,2,1);
    mainLayout->addWidget(wb,2,2);
//    QFrame *line = new QFrame();
//    line->setFrameShape(QFrame::HLine);
//    line->setFrameShadow(QFrame::Sunken);

}

UnionZero::~UnionZero()
{

}

void UnionZero::keyPressEvent(QKeyEvent *e)
{
    int i = 0;
    int tmp;
    int keyUnion[] = {Qt::Key_X,Qt::Key_Y,Qt::Key_C,Qt::Key_W,Qt::Key_A,Qt::Key_B};
    static DIGIT_CMD stDigitCmd;
    if (e->key() == Qt::Key_Escape)
        close();
    if (workflag == 0){
        //�ػ�е��
        tmp = e->key();
        for(i=0;i < 6;i++)
        {
            if(tmp == keyUnion[i])
            {
                edm->EdmRtZero(i);
            }
        }
        accept();
    }
    else{
        //�ع�����
        tmp = e->key();
        for(i=0;i < 6;i++)
        {
            if(tmp == keyUnion[i])
            {
                memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
                stDigitCmd.enAim = AIM_G90;
                stDigitCmd.enOrbit = ORBIT_G00;
                stDigitCmd.enCoor = edm->m_stEdmShowData.enCoorType;
                stDigitCmd.stOp.bShortDis = TRUE;
                stDigitCmd.iAxisCnt++;
                stDigitCmd.stAxisDigit[0].iDistance = 0;
                stDigitCmd.stAxisDigit[0].iLabel = i;
                edm->EdmSendMovePara(&stDigitCmd);
            }
        }
        accept();
    }
}
