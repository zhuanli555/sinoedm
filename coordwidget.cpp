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
    QString tmp2 = QString("%1").arg(zero,8,'f',3);
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

    xLabel->setAlignment(Qt::AlignVCenter);
    yLabel->setAlignment(Qt::AlignVCenter);
    zLabel->setAlignment(Qt::AlignVCenter);
    wLabel->setAlignment(Qt::AlignVCenter);
    aLabel->setAlignment(Qt::AlignVCenter);
    bLabel->setAlignment(Qt::AlignVCenter);
    xValue->setAlignment(Qt::AlignRight);
    yValue->setAlignment(Qt::AlignRight);
    zValue->setAlignment(Qt::AlignRight);
    wValue->setAlignment(Qt::AlignRight);
    aValue->setAlignment(Qt::AlignRight);
    bValue->setAlignment(Qt::AlignRight);
    xMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    yMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    zMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    wMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    aMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    bMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
    //yLabel->setStyleSheet("font-size:40px;border-image:url(bj.png);");
    xLabel->setStyleSheet(xstyle);
    yLabel->setStyleSheet(xstyle);
    zLabel->setStyleSheet(xstyle);
    wLabel->setStyleSheet(wstyle);
    aLabel->setStyleSheet(xstyle);
    bLabel->setStyleSheet(xstyle);
    xValue->setStyleSheet(xstyle);
    yValue->setStyleSheet(xstyle);
    zValue->setStyleSheet(xstyle);
    wValue->setStyleSheet(xstyle);
    aValue->setStyleSheet(xstyle);
    bValue->setStyleSheet(xstyle);
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
                str = QString("%1").arg(fTmp,8,'f',3);
                xMValue->setText(str);
                fTmp = (float)stMaccomm.stMoveCtrlComm[i].iRasilPos/1000.0;//相对坐标
                str = QString("%1").arg(fTmp,8,'f',3);
                xValue->setText(str);
            }else if (i == 1) {
                fTmp = (float)stMaccomm.stMoveCtrlComm[i].iMachPos/1000.0;//机械坐标
                str = QString("%1").arg(fTmp,8,'f',3);
                yMValue->setText(str);
                fTmp = (float)stMaccomm.stMoveCtrlComm[i].iRasilPos/1000.0;//相对坐标
                str = QString("%1").arg(fTmp,8,'f',3);
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

void CoordWidget::ShowMacUserStatus()
{
    QLabel* labels[MAC_LABEL_COUNT] = {xLabel,yLabel,nullptr,wLabel,aLabel,bLabel,zLabel};
    static unsigned long bPosLimit[MAC_LABEL_COUNT] = {0};
    static unsigned long bNegLimit[MAC_LABEL_COUNT] = {0};
    static unsigned long bAlarm[MAC_LABEL_COUNT] = {0};
    for (int i=0;i<MAC_LABEL_COUNT;i++)
    {
        if(i==2)continue;
        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bPosLimit != bPosLimit[i])
        {
            bPosLimit[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bPosLimit;
            if(bPosLimit[i])
            {
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px;border-image:url(zx.png);");
                }else{
                    labels[i]->setStyleSheet("font-size:40px;border-image:url(zx.png);");
                }

            }else{
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px");
                }else{
                    labels[i]->setStyleSheet("font-size:40px");
                }
            }
        }
        //负限
        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bNegLimit)
        {
            bNegLimit[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bNegLimit;
            if(bNegLimit[i])
            {
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px;border-image:url(fx.png);");
                }else{
                    labels[i]->setStyleSheet("font-size:40px;border-image:url(fx.png);");
                }

            }else{
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px");
                }else{
                    labels[i]->setStyleSheet("font-size:40px");
                }
            }
        }
        //轴报警

        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bAlarm)
        {
            bAlarm[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bAlarm;
            if(bAlarm[i])
            {
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px;border-image:url(bj.png);");
                }else{
                    labels[i]->setStyleSheet("font-size:40px;border-image:url(bj.png);");
                }

            }else{
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px");
                }else{
                    labels[i]->setStyleSheet("font-size:40px");
                }
            }
        }
    }
}

void CoordWidget::HandleEdmCycleData()
{
    edm->GetEdmComm();
    edm->GetEdmStatusData();
    SaveData();              //存储数据
    //SendComand2Edm();        //发送命令行数据
    ShowAxisData();          //显示数据
    ShowMacUserStatus();	 //状态显示
    edm->EdmAxisAdjust();
    edm->EdmAxisAdjustCircle();
}

