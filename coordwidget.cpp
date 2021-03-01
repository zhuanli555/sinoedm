#include "coordwidget.h"
#include <QDebug>
#include <QPixmap>

CoordWidget::CoordWidget(QWidget *parent) : QWidget(parent)
{
    edm = EDM::GetEdmInstance();
    setMaximumWidth(600);
    xLabel = new QLabel("X");
    yLabel = new QLabel("Y");
    zLabel = new QLabel("Z");
    wLabel = new QLabel("W");
    aLabel = new QLabel("A");
    bLabel = new QLabel("B");
    float zero = 0.000;
    QString tmp1 = QString("%1").arg("-123.567");
    QString tmp2 = QString("%1").arg(zero,8,'g',3);
    xValue = new QLabel(tmp1);
    yValue = new QLabel(tmp2);
    zValue = new QLabel(tmp2);
    xMValue = new QLabel(tmp1);
    yMValue = new QLabel(tmp2);
    zMValue = new QLabel(tmp2);

    wValue = new QLabel(tmp1);
    aValue = new QLabel(tmp2);
    bValue = new QLabel(tmp2);
    wMValue = new QLabel(tmp1);
    aMValue = new QLabel(tmp2);
    bMValue = new QLabel(tmp2);

    xValue->setAlignment(Qt::AlignRight);
    yValue->setAlignment(Qt::AlignRight);
    zValue->setAlignment(Qt::AlignRight);
    xMValue->setAlignment(Qt::AlignRight);
    yMValue->setAlignment(Qt::AlignRight);
    zMValue->setAlignment(Qt::AlignRight);
    wValue->setAlignment(Qt::AlignRight);
    aValue->setAlignment(Qt::AlignRight);
    bValue->setAlignment(Qt::AlignRight);
    wMValue->setAlignment(Qt::AlignRight);
    aMValue->setAlignment(Qt::AlignRight);
    bMValue->setAlignment(Qt::AlignRight);

    leftLayout = new QGridLayout(this);
    leftLayout->addWidget(xLabel,0,0);
    leftLayout->addWidget(xValue,0,1);
    leftLayout->addWidget(xMValue,0,2);
    leftLayout->addWidget(yLabel,1,0);
    leftLayout->addWidget(yValue,1,1);
    leftLayout->addWidget(yMValue,1,2);
    leftLayout->addWidget(zLabel,2,0);
    leftLayout->addWidget(zValue,2,1);
    leftLayout->addWidget(zMValue,2,2);
    leftLayout->addWidget(wLabel,3,0);
    leftLayout->addWidget(wValue,3,1);
    leftLayout->addWidget(wMValue,3,2);
    leftLayout->addWidget(aLabel,4,0);
    leftLayout->addWidget(aValue,4,1);
    leftLayout->addWidget(aMValue,4,2);
    leftLayout->addWidget(bLabel,5,0);
    leftLayout->addWidget(bValue,5,1);
    leftLayout->addWidget(bMValue,5,2);
    //setStyleSheet("QLabel{background-color:yellow}");
    QString xstyle = "font-size:40px;";
    QString wstyle = "font-size:28px;";
    xLabel->setStyleSheet(xstyle);
    yLabel->setStyleSheet("font-size:40px;border-image:url(bj.png);");
    zLabel->setStyleSheet("font-size:40px;border-image:url(fxbj.png);");
    wLabel->setStyleSheet(wstyle);
    aLabel->setStyleSheet("font-size:40px;border-image:url(bj.png);");
    bLabel->setStyleSheet("font-size:40px;border-image:url(zxbj.png);");
    xValue->setStyleSheet("font-size:40px;");
    yValue->setStyleSheet("font-size:40px;");
    zValue->setStyleSheet("font-size:40px;");
    wValue->setStyleSheet("font-size:40px;");
    aValue->setStyleSheet("font-size:40px;");
    bValue->setStyleSheet("font-size:40px;");
    xMValue->setStyleSheet("color:blue;font-size:24px;");
    yMValue->setStyleSheet("color:blue;font-size:24px;");
    zMValue->setStyleSheet("color:blue;font-size:24px;");
    wMValue->setStyleSheet("color:blue;font-size:24px;");
    aMValue->setStyleSheet("color:blue;font-size:24px;");
    bMValue->setStyleSheet("color:blue;font-size:24px;");
    xLabel->setMaximumWidth(85);
    yLabel->setMaximumWidth(85);
    zLabel->setMaximumWidth(85);
    wLabel->setMaximumWidth(85);
    aLabel->setMaximumWidth(85);
    bLabel->setMaximumWidth(85);
}

CoordWidget::~CoordWidget()
{

}

void CoordWidget::ShowAxisData()
{
    ShowData(edm->m_stEdmShowData.stComm,edm->m_stEdmShowData.iWorkPos);
}


void CoordWidget::ShowData(const MAC_COMMON& stMaccomm,int iRelLabel[])
{
    static MAC_COMMON stMaccomLast;
    static int iRelLabelLast[MAC_LABEL_COUNT] = {0};
    float fTmp;
    QString str;
    memset(&stMaccomLast,0,sizeof(MAC_COMMON));
    for (int i=0;i<MAC_LABEL_COUNT;i++)
    {
        if (stMaccomm.stMoveCtrlComm[i].iMachPos != stMaccomLast.stMoveCtrlComm[i].iMachPos
            || stMaccomm.stMoveCtrlComm[i].iRasilPos != stMaccomLast.stMoveCtrlComm[i].iRasilPos)
        {

            stMaccomLast.stMoveCtrlComm[i].iMachPos = stMaccomm.stMoveCtrlComm[i].iMachPos;
            stMaccomLast.stMoveCtrlComm[i].iRasilPos = stMaccomm.stMoveCtrlComm[i].iRasilPos;
            iRelLabelLast[i]= iRelLabel[i];
            if(i == 0)
            {
                fTmp = (float)stMaccomm.stMoveCtrlComm[i].iMachPos/1000.0;//机械坐标
                str = QString("%1").arg(fTmp,8,'g',3);
                xMValue->setText(str);
                fTmp = (float)stMaccomm.stMoveCtrlComm[i].iRasilPos/1000.0;//相对坐标
                str = QString("%1").arg(fTmp,8,'g',3);
                xValue->setText(str);
            }else if (i == 1) {
                fTmp = (float)stMaccomm.stMoveCtrlComm[i].iMachPos/1000.0;//机械坐标
                str = QString("%1").arg(fTmp,8,'g',3);
                yMValue->setText(str);
                fTmp = (float)stMaccomm.stMoveCtrlComm[i].iRasilPos/1000.0;//相对坐标
                str = QString("%1").arg(fTmp,8,'g',3);
                yValue->setText(str);
            }
        }

    }
}

void CoordWidget::SaveData()
{
    static MAC_COMMON stMaccomLast;
    static int iSaveCnt = 0;
    unsigned char bSave = FALSE;
    memset(&stMaccomLast,0,sizeof(MAC_COMMON));
    if (++iSaveCnt>=5)
    {
        //数据写入
        iSaveCnt = 0;
        for (int i=0;i<MAC_LABEL_COUNT;i++)
        {
            if ( edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iMachPos != stMaccomLast.stMoveCtrlComm[i].iMachPos
                || edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].bDirMove != stMaccomLast.stMoveCtrlComm[i].bDirMove
                || edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iWorkPosSet != stMaccomLast.stMoveCtrlComm[i].iWorkPosSet)
            {
                bSave = TRUE;
                break;
            }
        }
        if (bSave)
        {
            memcpy(&stMaccomLast,&edm->m_stEdmShowData.stComm,sizeof(MAC_COMMON));
            edm->EdmSaveMacComm();
        }
        bSave = FALSE;
    }
}

void CoordWidget::HandleEdmCycleData()
{
    edm->GetEdmStatusData();
    SaveData();              //存储数据
    //SendComand2Edm();        //发送命令行数据
    ShowAxisData();          //显示数据
    //ShowMacUserStatus();	 //状态显示
    //EdmHandKeyProcess();
    edm->EdmAxisAdjust();
    edm->EdmAxisAdjustCircle();
}

