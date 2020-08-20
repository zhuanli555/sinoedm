#include "unionzero.h"
#include <QtGui/QKeyEvent>
#include <QTextCodec>

UnionZero::UnionZero(QWidget *parent) : QDialog(parent)
{
    setGeometry(400,100,300,400);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    setWindowTitle(QString::fromLocal8Bit("轴回零位"));
    setStyleSheet("QLabel{\
          border:3px solid rgb(60,60,60);margin:2px 2px;}");
    edm = EDM::GetEdmInstance();

    esc = new QLabel(QString::fromLocal8Bit("返回\n(Esc)"));
    w = new QLabel(QString::fromLocal8Bit("轴回工作零位"));
    wx =  new QLabel(QString::fromLocal8Bit("选择x轴\n(x)"));
    wy =  new QLabel(QString::fromLocal8Bit("选择y轴\n(y)"));
    ww =  new QLabel(QString::fromLocal8Bit("选择w轴\n(w)"));
    wc =  new QLabel(QString::fromLocal8Bit("选择c轴\n(c)"));
    wa =  new QLabel(QString::fromLocal8Bit("选择a轴\n(a)"));
    wb =  new QLabel(QString::fromLocal8Bit("选择b轴\n(b)"));
    m = new QLabel(QString::fromLocal8Bit("轴回机械零位"));
    mx =  new QLabel(QString::fromLocal8Bit("选择x轴\n(Ctrl-x)"));
    my =  new QLabel(QString::fromLocal8Bit("选择y轴\n(Ctrl-y)"));
    mw =  new QLabel(QString::fromLocal8Bit("选择w轴\n(Ctrl-w)"));
    mc =  new QLabel(QString::fromLocal8Bit("选择c轴\n(Ctrl-c)"));
    ma =  new QLabel(QString::fromLocal8Bit("选择a轴\n(Ctrl-a)"));
    mb =  new QLabel(QString::fromLocal8Bit("选择b轴\n(Ctrl-b)"));
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(esc,0,0,1,1);
    mainLayout->addWidget(w,0,1,1,2);
    mainLayout->addWidget(wx,1,0);
    mainLayout->addWidget(wy,1,1);
    mainLayout->addWidget(ww,1,2);
    mainLayout->addWidget(wc,2,0);
    mainLayout->addWidget(wa,2,1);
    mainLayout->addWidget(wb,2,2);
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line,3,0,1,3);
    mainLayout->addWidget(m,4,1,1,2);
    mainLayout->addWidget(mx,5,0);
    mainLayout->addWidget(my,5,1);
    mainLayout->addWidget(mw,5,2);
    mainLayout->addWidget(mc,6,0);
    mainLayout->addWidget(ma,6,1);
    mainLayout->addWidget(mb,6,2);

}

UnionZero::~UnionZero()
{

}

void UnionZero::keyPressEvent(QKeyEvent *e)
{
    int i = 0;
    int tmp;
    int keyUnion[] = {Qt::Key_X,Qt::Key_Y,Qt::Key_W,Qt::Key_C,Qt::Key_A,Qt::Key_B};
    static DIGIT_CMD stDigitCmd;
    if (e->key() == Qt::Key_Escape)
        close();
    if (e->modifiers() == Qt::ControlModifier){
        //回机械零
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
        //回工作零
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
